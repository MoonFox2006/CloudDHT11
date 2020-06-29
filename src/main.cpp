extern "C" {
#include "user_interface.h"

extern struct rst_info resetInfo;
}
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "Customization.h"
#include "Config.h"
#ifdef USE_LED
#include "Led.h"
#endif
#include "CaptivePortal.h"
#include "RtcData.h"
#include "DHT.h"
#include "Date.h"
#include "Debug.h"
#include "StrUtils.h"
#include "LightSleep.h"

const uint32_t RTC_FLAG_CP = 1;

const uint32_t DIAG_TIME = 2000; // 2 sec.

const uint32_t EPOCH_2020 = 1577836800; // 01.01.2020 in seconds

Config config;
RtcData rtc;
#ifdef USE_LED
Led led(2, LOW);
#endif
DHT dht(2, DHT::DHT11);

static const char OK_PSTR[] PROGMEM = "OK";
static const char FAIL_PSTR[] PROGMEM = "FAIL!";

void cleanup() {
#ifdef USE_SERIAL
  Serial.flush();
#endif
#ifdef USE_LED
  led.setMode(Led::LED_OFF);
#endif
}

static void wakeup() {
  DEBUG_LOGV("Waked up");
}

static void halt(const __FlashStringHelper *msg) {
#ifdef USE_SERIAL
  Serial.println();
  Serial.println(msg);
  Serial.println(F("System halted!"));
#endif
#ifdef USE_LED
  led.setMode(Led::LED_4HZ);
  led.delay(DIAG_TIME);
#endif
  cleanup();
  ESP.deepSleep(0);
}

static void restart(const __FlashStringHelper *msg) {
#ifdef USE_SERIAL
  Serial.println();
  Serial.println(msg);
  Serial.println(F("System restarting..."));
#endif
  cleanup();
  ESP.restart();
}

static void sleep(uint32_t ms) {
  lightSleep(ms, cleanup, wakeup);
}

static bool wifiConnect() {
  const uint32_t WIFI_TIMEOUT = 30000; // 30 sec.

  uint32_t start;

  WiFi.mode(WIFI_STA);
  if (config.wifi_wep)
    WiFi.enableInsecureWEP(true);
  WiFi.begin(config.wifi_ssid, config.wifi_pswd);
#ifdef USE_SERIAL
  Serial.print(F("Connecting to SSID \""));
  Serial.print(config.wifi_ssid);
  Serial.println(F("\"..."));
#endif
#ifdef USE_LED
  led.setMode(Led::LED_1HZ);
#endif
  start = millis();
  while ((! WiFi.isConnected()) && (millis() - start < WIFI_TIMEOUT)) {
#ifdef USE_LED
    led.delay(500);
#else
    delay(500);
#endif
  }
#ifdef USE_LED
  led.setMode(Led::LED_OFF);
#endif
  if (WiFi.isConnected()) {
#ifdef USE_SERIAL
    Serial.print(F("OK (IP: "));
    Serial.print(WiFi.localIP());
    Serial.println(')');
#endif
    return true;
  } else {
#ifdef USE_SERIAL
    Serial.println(FPSTR(FAIL_PSTR));
#endif
    return false;
  }
}

static uint32_t ntpTime(const char *server, int8_t tz, uint32_t timeout) {
  const uint8_t NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
  const uint32_t SEVENTY_YEARS = 2208988800UL;

  uint32_t result = 0;
  uint8_t buf[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
  WiFiUDP udp;
  uint32_t time;

  memset(buf, 0, sizeof(buf));
  // Initialize values needed to form NTP request
  buf[0] = 0B11100011; // LI, Version, Mode
  buf[1] = 0; // Stratum, or type of clock
  buf[2] = 6; // Polling Interval
  buf[3] = 0xEC; // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  buf[12] = 49;
  buf[13] = 0x4E;
  buf[14] = 49;
  buf[15] = 52;

  if ((! udp.begin(random(65536 - 1024) + 1024)) || (! udp.beginPacket(server, 123)) || (udp.write(buf, sizeof(buf)) != sizeof(buf)) || (! udp.endPacket()))
    return 0;

  time = millis();

  while (! udp.parsePacket()) {
    if (millis() - time > timeout)
      return 0;
    delay(1);
  }
  udp.read(buf, sizeof(buf));
  result = ((uint32_t)buf[40] << 24) | ((uint32_t)buf[41] << 16) | ((uint32_t)buf[42] << 8) | buf[43];
  result -= SEVENTY_YEARS;
  result += tz * 3600;

  return result;
}

static bool sendQuery(const String &query, String &response) {
  WiFiClientSecure client;
  HTTPClient http;
  bool result = false;

  client.setInsecure();
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.setTimeout(15000);
  if (system_get_cpu_freq() < SYS_CPU_160MHZ) {
    system_update_cpu_freq(SYS_CPU_160MHZ);
    DEBUG_LOGV("Switch CPU frequency to 160 MHz");
  }
  if (http.begin(client, config.script_host, config.script_port, config.script_uri, true)) {
    int16_t code;

    code = http.POST(query);
    if (code < 0) {
      DEBUG_LOGW("HTTPClient POST error %d!", code);
    } else {
      response = http.getString();
      result = (code == 200) || (code == 201);
      DEBUG_LOGD("HTTPClient POST code %d, response \"%s\"", code, response.c_str());
    }
    http.end();
  }
  if (system_get_cpu_freq() > SYS_CPU_80MHZ) {
    system_update_cpu_freq(SYS_CPU_80MHZ);
    DEBUG_LOGV("Revert CPU frequency");
  }

  return result;
}

static bool sendConfig() {
  String query, response;

  query = printfToString(PSTR("{\"uid\":\"%0.8lX\",\"config\":"), ESP.getChipId());
  query += config.toString();
  query += '}';
  if (sendQuery(query, response)) {
    if (response[0] == '{') { // New configuration returned
      if (config.fromString(response)) {
        if (config.save()) {
          DEBUG_LOGI("New config saved.");
        } else {
          DEBUG_LOGE("Error saving new config!");
        }
      } else {
        config.load();
        DEBUG_LOGE("Error applying new config!");
      }
    }
    return true;
  }
  return false;
}

static bool sendDataPacket(const String &query) {
  String response;

  return sendQuery(query, response) && (! strcmp_P(response.c_str(), PSTR("OK")));
}

static void concatData(String &query, const RtcData::data_t &d, bool start = false, bool end = false) {
  static const char NULL_PSTR[] PROGMEM = "null";
  static const char FLOAT2_PSTR[] PROGMEM = "%0.2f";

  if (start) {
    query = printfToString(PSTR("{\"uid\":\"%0.8lX\",\"rssi\":%ld,\"samples\":["), ESP.getChipId(), WiFi.RSSI());
  } else {
    query += ',';
  }
  query += printfToString(PSTR("{\"timestamp\":%lu"), d.timestamp - config.ntp_tz * 3600); // UTC time
  query += F(",\"temp\":");
  if (! isnan(d.temp))
    query += printfToString(FLOAT2_PSTR, d.temp);
  else
    query += FPSTR(NULL_PSTR);
  query += F(",\"hum\":");
  if (! isnan(d.humidity))
    query += printfToString(FLOAT2_PSTR, d.humidity);
  else
    query += FPSTR(NULL_PSTR);
  query += '}';
  if (end)
    query += F("]}");
}

static bool sendData(const RtcData::data_t &d) {
  if (! WiFi.isConnected())
    return false;

  bool result = sendConfig();

  if (result) {
    String query;
    uint8_t ind, cnt, sended;

    ind = 0;
    cnt = 0;
    sended = 0;
    while (ind < rtc.count()) {
      concatData(query, rtc[ind], cnt == 0);
      if (++cnt >= 4) {
        query += F("]}");
        if (! sendDataPacket(query)) {
          result = false;
          break;
        }
        sended += cnt;
        cnt = 0;
      }
      ++ind;
    }
    if (result) {
      concatData(query, d, cnt == 0, true);
      result = sendDataPacket(query);
      if (result) {
        sended += cnt;
      }
    }
    if (sended) {
      rtc.remove(sended);
    }
  } else {
    DEBUG_LOGE("Error refreshing config from cloud!");
  }

  return result;
}

static void payload() {
  static uint16_t measured = 0;

  uint32_t start = millis();
  RtcData::data_t d;
  bool error = false;

  if (rtc.count()) {
    for (int8_t i = rtc.count() - 1; i >= 0; --i) {
      if (rtc[i].timestamp < EPOCH_2020)
        rtc[i].timestamp += config.measure_period;
    }
    rtc.update();
  }

  if (dht.measure(&d.temp, &d.humidity) || dht.measure(&d.temp, &d.humidity)) { // Try 2 measurements
#ifdef USE_SERIAL
    if (! isnan(d.temp)) {
      Serial.print(F("Temperature is "));
      Serial.print(d.temp);
      Serial.println(F(" C"));
    }
    if (! isnan(d.humidity)) {
      Serial.print(F("Humidity is "));
      Serial.print(d.humidity);
      Serial.println(F(" %"));
    }
#endif
    if ((++measured >= config.send_every) || (! rtc.checkTolerance(d.temp, d.humidity, config.temp_tolerance, config.hum_tolerance))) {
      rtc.setLastData(d.temp, d.humidity);
      measured = 0;
      d.timestamp = 0;
      if (wifiConnect()) {
        uint32_t time = 0;

        if (*config.ntp_server) {
          uint8_t repeat = 4;

          do {
            time = ntpTime(config.ntp_server, config.ntp_tz, 1000);
          } while ((! time) && repeat--);
          if (time) {
            time -= (millis() - start) / 1000;
          }
        }
        if (time) {
          d.timestamp = time;
          if (rtc.count()) {
            for (int8_t i = rtc.count() - 1; i >= 0; --i) {
              if (rtc[i].timestamp < EPOCH_2020)
                rtc[i].timestamp = time - rtc[i].timestamp;
            }
            rtc.update();
          }

#ifdef USE_SERIAL
          {
            char timestr[20];

            Serial.print(F("Now "));
            Serial.println(dateTimeToStr(timestr, time));
          }
#endif
/*
#ifdef USE_LED
          led.setMode(Led::LED_ON);
#endif
*/
          if (sendData(d)) {
            DEBUG_LOGI("Data sended to cloud successfully");
          } else {
            error = true;
            DEBUG_LOGE("Error sending data to cloud!");
          }
/*
#ifdef USE_LED
          led.setMode(Led::LED_OFF);
#endif
*/
        } else {
          error = true;
          DEBUG_LOGE("Unable to get current time!");
        }
      } else { // Error connecting to WiFi
        error = true;
      }
      WiFi.disconnect();
      if (error) {
        if (! rtc.add(d)) {
          DEBUG_LOGE("Error storing RTC data!");
        }
      }
    }
  } else {
    error = true;
    DEBUG_LOGE("DHT11 read error!");
  }

#ifdef USE_LED
  if (error) {
    led.setMode(Led::LED_4HZ);
    led.delay(DIAG_TIME);
  }
#endif
#ifdef USE_SERIAL
  Serial.println(F("Going to sleep..."));
#endif
  start = millis() - start;
  if (start < config.measure_period * 1000)
    start = config.measure_period * 1000 - start;
  else
    start = config.measure_period * 1000;
  sleep(start);
}

static bool isConfigIncomplete() {
  return (! *config.wifi_ssid) || (! *config.ntp_server) || (! *config.script_host) || (! *config.script_uri);
}

void setup() {
#ifdef USE_SERIAL
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  Serial.println();
#endif

  WiFi.persistent(false);

  if (! initSPIFFS()) {
    halt(F("SPIFFS init fail!"));
  }

  if (! config.load()) {
    config.clear();
    DEBUG_LOGI("Use default configuration");
  }

  if (isConfigIncomplete() || (! rtc.getFlag(RTC_FLAG_CP))) {
#ifdef USE_SERIAL
#ifdef USE_LED
    CaptivePortal cp(&config, led, &Serial);
#else
    CaptivePortal cp(&config, &Serial);
#endif
#else
#ifdef USE_LED
    CaptivePortal cp(&config, led);
#else
    CaptivePortal cp(&config);
#endif
#endif

    rtc.setFlag(RTC_FLAG_CP);
    cp.exec(isConfigIncomplete() ? 0 : 60);
  }
  if (isConfigIncomplete())
    halt(F("Configuration incomplete!"));
  rtc.clearFlag(RTC_FLAG_CP);
  rtc.clear();

  {
    const uint8_t MAX_ERRORS = 5;

    uint8_t errors = 0;
    float temp[2], hum[2];

#ifdef USE_SERIAL
    Serial.println(F("Waiting for DHT11 calibration completed..."));
#endif
    do {
      delay(2000);
      if (! dht.measure(&temp[0], &hum[0])) {
        if (++errors >= MAX_ERRORS)
          halt(F("DHT11 not found or damaged!"));
      } else
        errors = 0;
    } while (errors > 0);
    while (true) {
      delay(2000);
      if (! dht.measure(&temp[1], &hum[1])) {
        if (++errors >= MAX_ERRORS)
          halt(F("DHT11 too many read errors!"));
      } else {
        errors = 0;
        if ((temp[1] == temp[0]) && (hum[1] == hum[0]))
          break;
        temp[0] = temp[1];
        hum[0] = hum[1];
      }
    }
  }
}

void loop() {
  payload();
}
