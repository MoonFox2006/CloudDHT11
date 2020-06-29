#ifndef __CONFIG_H
#define __CONFIG_H

#include "Customization.h"
#include "BaseConfig.h"

static const char WIFI_SSID_PARAM[] PROGMEM = "wifi_ssid";
static const char WIFI_SSID_DESCR[] PROGMEM = "WiFi SSID";
#ifdef WIFI_SSID
static const char WIFI_SSID_DEF[] PROGMEM = WIFI_SSID;
#endif
static const char WIFI_PSWD_PARAM[] PROGMEM = "wifi_pswd";
static const char WIFI_PSWD_DESCR[] PROGMEM = "WiFi password";
#ifdef WIFI_PSWD
static const char WIFI_PSWD_DEF[] PROGMEM = WIFI_PSWD;
#endif
static const char WIFI_WEP_PARAM[] PROGMEM = "wifi_wep";
static const char WIFI_WEP_DESCR[] PROGMEM = "WiFi insecure (WEP)?";
static const bool WIFI_WEP_DEF = false;
static const char NTP_SERVER_PARAM[] PROGMEM = "ntp_server";
static const char NTP_SERVER_DESCR[] PROGMEM = "NTP server";
static const char NTP_SERVER_DEF[] PROGMEM = NTP_SERVER;
static const char NTP_TZ_PARAM[] PROGMEM = "ntp_tz";
static const char NTP_TZ_DESCR[] PROGMEM = "Time zone";
static const int8_t NTP_TZ_DEF = NTP_TZ;
static const char SCRIPT_HOST_PARAM[] PROGMEM = "script_host";
static const char SCRIPT_HOST_DESCR[] PROGMEM = "Script host";
static const char SCRIPT_HOST_DEF[] PROGMEM = "script.google.com";
static const char SCRIPT_PORT_PARAM[] PROGMEM = "script_port";
static const char SCRIPT_PORT_DESCR[] PROGMEM = "Script port";
static const uint16_t SCRIPT_PORT_DEF = 443;
static const char SCRIPT_URI_PARAM[] PROGMEM = "script_uri";
static const char SCRIPT_URI_DESCR[] PROGMEM = "Script URL";
#ifdef GSCRIPT_URI
static const char SCRIPT_URI_DEF[] PROGMEM = GSCRIPT_URI;
#endif
static const char TEMP_TOLERANCE_PARAM[] PROGMEM = "temp_tolerance";
static const char TEMP_TOLERANCE_DESCR[] PROGMEM = "Temperature tolerance";
static const float TEMP_TOLERANCE_DEF = 2.0;
static const char HUM_TOLERANCE_PARAM[] PROGMEM = "hum_tolerance";
static const char HUM_TOLERANCE_DESCR[] PROGMEM = "Humidity tolerance";
static const float HUM_TOLERANCE_DEF = 5.0;
static const char MEASURE_PERIOD_PARAM[] PROGMEM = "measure_period";
static const char MEASURE_PERIOD_DESCR[] PROGMEM = "Measure period (in sec.)";
static const uint16_t MEASURE_PERIOD_DEF = 60;
static const char SEND_EVERY_PARAM[] PROGMEM = "send_every";
static const char SEND_EVERY_DESCR[] PROGMEM = "Sending every Nth measurement";
static const uint16_t SEND_EVERY_DEF = 15;

static const uint8_t WIFI_MAX_LEN = 32;
static const uint8_t HOST_MAX_LEN = 32;
static const uint8_t URI_MAX_LEN = 128;

static const param_t PARAMS[] PROGMEM = {
#ifdef WIFI_SSID
  PARAM_STR(WIFI_SSID_PARAM, WIFI_SSID_DESCR, WIFI_MAX_LEN, WIFI_SSID_DEF),
#else
  PARAM_STR(WIFI_SSID_PARAM, WIFI_SSID_DESCR, WIFI_MAX_LEN, NULL),
#endif
#ifdef WIFI_PSWD
  PARAM_PSWD(WIFI_PSWD_PARAM, WIFI_PSWD_DESCR, WIFI_MAX_LEN, WIFI_PSWD_DEF),
#else
  PARAM_PSWD(WIFI_PSWD_PARAM, WIFI_PSWD_DESCR, WIFI_MAX_LEN, NULL),
#endif
  PARAM_BOOL(WIFI_WEP_PARAM, WIFI_WEP_DESCR, WIFI_WEP_DEF),
  PARAM_STR(NTP_SERVER_PARAM, NTP_SERVER_DESCR, HOST_MAX_LEN, NTP_SERVER_DEF),
  PARAM_I8(NTP_TZ_PARAM, NTP_TZ_DESCR, NTP_TZ_DEF),
  PARAM_STR(SCRIPT_HOST_PARAM, SCRIPT_HOST_DESCR, HOST_MAX_LEN, SCRIPT_HOST_DEF),
  PARAM_UI16(SCRIPT_PORT_PARAM, SCRIPT_PORT_DESCR, SCRIPT_PORT_DEF),
#ifdef GSCRIPT_URI
  PARAM_STR(SCRIPT_URI_PARAM, SCRIPT_URI_DESCR, URI_MAX_LEN, SCRIPT_URI_DEF),
#else
  PARAM_STR(SCRIPT_URI_PARAM, SCRIPT_URI_DESCR, URI_MAX_LEN, NULL),
#endif
  PARAM_FLOAT(TEMP_TOLERANCE_PARAM, TEMP_TOLERANCE_DESCR, TEMP_TOLERANCE_DEF),
  PARAM_FLOAT(HUM_TOLERANCE_PARAM, HUM_TOLERANCE_DESCR, HUM_TOLERANCE_DEF),
  PARAM_UI16(MEASURE_PERIOD_PARAM, MEASURE_PERIOD_DESCR, MEASURE_PERIOD_DEF),
  PARAM_UI16(SEND_EVERY_PARAM, SEND_EVERY_DESCR, SEND_EVERY_DEF)
};

class Config : public BaseConfig {
public:
  Config() : BaseConfig(PARAMS, sizeof(PARAMS) / sizeof(PARAMS[0])) {}

  void *getParamPtr(uint8_t index);

  char wifi_ssid[WIFI_MAX_LEN];
  char wifi_pswd[WIFI_MAX_LEN];
  char ntp_server[HOST_MAX_LEN];
  char script_host[HOST_MAX_LEN];
  char script_uri[URI_MAX_LEN];
  float temp_tolerance;
  float hum_tolerance;
  uint16_t script_port;
  uint16_t measure_period;
  uint16_t send_every;
  int8_t ntp_tz;
  bool wifi_wep;
};

#endif
