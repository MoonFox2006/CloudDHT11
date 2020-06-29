#ifndef __DHT_H
#define __DHT_H

#include <inttypes.h>

class DHT {
public:
  enum sensortype_t { DHT11, DHT21, DHT22 };

  DHT(uint8_t pin, sensortype_t type) : _pin(pin), _type(type) {}

  bool measure(float *temperature, float *humidity);

protected:
  struct __packed {
    uint8_t _pin : 6;
    sensortype_t _type : 2;
  };
};

#endif
