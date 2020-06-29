#include <Arduino.h>
#include "DHT.h"

bool DHT::measure(float *temperature, float *humidity) {
  uint8_t data[5];
  int8_t i;

  for (i = 0; i < 5; ++i) {
    data[i] = 0;
  }
  if (temperature)
    *temperature = NAN;
  if (humidity)
    *humidity = NAN;

  digitalWrite(_pin, LOW);
  pinMode(_pin, OUTPUT);
  if (_type == DHT11)
    delay(18);
  else
    delayMicroseconds(800);

  pinMode(_pin, INPUT_PULLUP);
  // 83 edges:
  // - First a FALLING, RISING, and FALLING edge for the start bit
  // - Then 40 bits: RISING and then a FALLING edge per bit
  // To keep our code simple, we accept any HIGH or LOW reading if it's max 85 usecs long

  cli();
  for (i = -3 ; i < 2 * 40; ++i) {
    uint32_t startTime = micros();
    uint8_t length;

    do {
      length = (uint32_t)(micros() - startTime);
      if (length > 90) {
        sei();
        return false;
      }
    } while (digitalRead(_pin) == (i & 1));

    if ((i >= 0) && (i & 1)) {
      data[i / 16] <<= 1;

      // A zero max 30 usecs, a one at least 68 usecs.
      if (length > 30) {
        data[i / 16] |= 1; // we got a one
      }
    }
  }
  sei();

  if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4]) {
    return false;
  }

  if (_type == DHT11) {
    if (humidity) {
      *humidity = data[0];
    }
    if (temperature) {
      *temperature = data[2];
    }
  } else {
    if (humidity) {
      *humidity = (((uint16_t)data[0] << 8) | data[1]) * 0.1;
    }
    if (temperature) {
      *temperature = (((uint16_t)(data[2] & 0x7F) << 8) | data[3]) * 0.1;
      if (data[2] & 0x80)
        *temperature = -*temperature;
    }
  }

  return true;
}
