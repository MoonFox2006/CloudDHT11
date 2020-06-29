#include <Arduino.h>
#include "RtcData.h"

bool RtcData::add(const data_t &data) {
  if (_cnt < MAX_COUNT) {
    ++_cnt;
  } else {
    memmove(&_rtc.data[0], &_rtc.data[1], sizeof(data_t) * (MAX_COUNT - 1));
  }
  memcpy(&_rtc.data[_cnt - 1], &data, sizeof(data_t));

  return write();
}

bool RtcData::add(uint32_t timestamp, float temp, float humidity) {
  data_t data;

  data.timestamp = timestamp;
  data.temp = temp;
  data.humidity = humidity;

  return add(data);
}

void RtcData::remove(uint8_t cnt) {
  if (_cnt) {
    if (cnt < _cnt) {
      _cnt -= cnt;
      memmove(&_rtc.data[0], &_rtc.data[cnt], sizeof(data_t) * _cnt);
    } else {
      _cnt = 0;
    }
    write();
  }
}

void RtcData::clear() {
  memset(_rtc.data, 0, sizeof(_rtc.data));
  _rtc.lastTemp = NAN;
  _rtc.lastHumidity = NAN;
  _cnt = 0;
  write();
}

bool RtcData::setLastData(float temp, float humidity) {
  _rtc.lastTemp = temp;
  _rtc.lastHumidity = humidity;
  return write();
}

bool RtcData::checkTolerance(float temp, float humidity, float tempTolerance, float humTolerance) {
  bool result;

  result = ((! isnan(_rtc.lastTemp)) && (fabs(temp - _rtc.lastTemp) < tempTolerance));
  if (result)
    result = ((! isnan(_rtc.lastHumidity)) && (fabs(humidity - _rtc.lastHumidity) < humTolerance));
  return result;
}

uint16_t RtcData::crc16(const uint8_t *data, uint16_t size, uint16_t crc) {
  while (size--) {
    crc ^= *data++ << 8;
    for (uint8_t i = 0; i < 8; ++i)
      crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
  }

  return crc;
}

bool RtcData::read() {
  header_t header;

  if (ESP.rtcUserMemoryRead(0, (uint32_t*)&header, sizeof(header)) && (header.signature == SIGNATURE) &&
    ((uint8_t)~header.cnt == header.ncnt) && (header.cnt <= MAX_COUNT)) {
    if (ESP.rtcUserMemoryRead(sizeof(header_t) / 4, (uint32_t*)&_rtc, sizeof(_rtc)) && (header.crc == crc16((uint8_t*)&_rtc, sizeof(_rtc)))) {
      _cnt = header.cnt;
      return true;
    }
  }
  memset(_rtc.data, 0, sizeof(_rtc.data));
  _rtc.flags = 0;
  _rtc.lastTemp = NAN;
  _rtc.lastHumidity = NAN;
  _cnt = 0;
  return false;
}

bool RtcData::write() {
  header_t header;

  header.signature = SIGNATURE;
  header.cnt = _cnt;
  header.ncnt = ~header.cnt;
  header.crc = crc16((uint8_t*)&_rtc, sizeof(_rtc));

  return ESP.rtcUserMemoryWrite(0, (uint32_t*)&header, sizeof(header)) && ESP.rtcUserMemoryWrite(sizeof(header_t) / 4, (uint32_t*)&_rtc, sizeof(_rtc));
}
