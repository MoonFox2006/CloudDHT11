#ifndef __RTCDATA_H
#define __RTCDATA_H

#include <inttypes.h>

class RtcData {
public:
  struct __packed data_t {
    uint32_t timestamp;
    float temp;
    float humidity;
  };

  RtcData() {
    read();
  }

  uint8_t count() const {
    return _cnt;
  }
  bool add(const data_t &data);
  bool add(uint32_t timestamp, float temp, float humidity);
  void remove(uint8_t cnt = 1);
  void clear();
  bool update() {
    return write();
  }
  data_t &get(uint8_t index) {
    return _rtc.data[index];
  }
  data_t &operator[](uint8_t index) {
    return get(index);
  }
  data_t &last() {
    return _rtc.data[_cnt - 1];
  }
  uint32_t getFlags() const {
    return _rtc.flags;
  }
  bool getFlag(uint32_t value) const {
    return (_rtc.flags & value) != 0;
  }
  bool setFlags(uint32_t value) {
    _rtc.flags = value;
    return write();
  }
  bool setFlag(uint32_t value) {
    _rtc.flags |= value;
    return write();
  }
  bool clearFlag(uint32_t value) {
    _rtc.flags &= ~value;
    return write();
  }
  bool setLastData(float temp, float humidity);
  bool checkTolerance(float temp, float humidity, float tempTolerance, float humTolerance);

protected:
  struct __packed header_t {
    uint32_t signature;
    uint8_t cnt;
    uint8_t ncnt;
    uint16_t crc;
  };

  static uint16_t crc16(const uint8_t *data, uint16_t size, uint16_t crc = 0xFFFF);

  bool read();
  bool write();

  static const uint32_t SIGNATURE = 0xA1B2C3D4;
  static const uint8_t MAX_COUNT = (512 - sizeof(header_t) - sizeof(uint32_t) - sizeof(float) * 2) / sizeof(data_t);

  struct __packed {
    data_t data[MAX_COUNT];
    uint32_t flags;
    float lastTemp;
    float lastHumidity;
  } _rtc;
  uint8_t _cnt;
};

#endif
