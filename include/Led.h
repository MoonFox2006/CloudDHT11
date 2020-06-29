#ifndef __LED_H
#define __LED_H

#include <inttypes.h>

//#define HALF_BRIGHT

class Led {
public:
  enum ledmode_t { LED_OFF, LED_ON, LED_TOGGLE, LED_1HZ, LED_2HZ, LED_4HZ, LED_FADEIN, LED_FADEOUT, LED_FADEINOUT };

  Led(uint8_t pin, bool level = false);

  ledmode_t getMode() const {
    return _mode;
  }
  void setMode(ledmode_t mode) {
    _mode = mode;
    update(true);
  }
  void update(bool force = false);
  void delay(uint32_t ms, uint32_t step = 1);

protected:
  void on();
  void off() {
    digitalWrite(_pin, ! _level);
    _on = false;
  }

  struct __attribute__((__packed__)) {
    uint8_t _pin;
    bool _level : 1;
    ledmode_t _mode : 6;
    bool _on : 1;
  };
};

#endif
