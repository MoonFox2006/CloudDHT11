#include <Arduino.h>
#include "Led.h"

Led::Led(uint8_t pin, bool level) : _pin(pin), _level(level), _mode(LED_OFF) {
  pinMode(_pin, OUTPUT);
  off();
}

void Led::update(bool force) {
  const uint32_t BLINK_TIME = 10;

  if (force || (_mode > LED_ON)) {
    pinMode(_pin, OUTPUT);
    if (_mode == LED_OFF) {
      off();
    } else if (_mode == LED_ON) {
      on();
    } else if (_mode == LED_TOGGLE) {
      if (_on)
        off();
      else
        on();
    } else {
      uint16_t subsec;

      if (_mode == LED_FADEINOUT)
        subsec = millis() % 2000;
      else
        subsec = millis() % 1000;
      if (_mode == LED_1HZ) {
        if (subsec < BLINK_TIME)
          on();
        else
          off();
      } else if (_mode == LED_2HZ) {
        if (subsec < BLINK_TIME)
          on();
        else if (subsec < 500)
          off();
        else if (subsec < 500 + BLINK_TIME)
          on();
        else
          off();
      } else if (_mode == LED_4HZ) {
        if (subsec < BLINK_TIME)
          on();
        else if (subsec < 250)
          off();
        else if (subsec < 250 + BLINK_TIME)
          on();
        else if (subsec < 500)
          off();
        else if (subsec < 500 + BLINK_TIME)
          on();
        else if (subsec < 750)
          off();
        else if (subsec < 750 + BLINK_TIME)
          on();
        else
          off();
      } else if (_mode == LED_FADEIN) {
        if (_level)
#ifdef HALF_BRIGHT
          analogWrite(_pin, map(subsec, 0, 999, 0, 511));
#else
          analogWrite(_pin, map(subsec, 0, 999, 0, 1023));
#endif
        else
#ifdef HALF_BRIGHT
          analogWrite(_pin, map(subsec, 0, 999, 1023, 512));
#else
          analogWrite(_pin, map(subsec, 0, 999, 1023, 0));
#endif
      } else if (_mode == LED_FADEOUT) {
        if (_level)
#ifdef HALF_BRIGHT
          analogWrite(_pin, map(subsec, 0, 999, 511, 0));
#else
          analogWrite(_pin, map(subsec, 0, 999, 1023, 0));
#endif
        else
#ifdef HALF_BRIGHT
          analogWrite(_pin, map(subsec, 0, 999, 512, 1023));
#else
          analogWrite(_pin, map(subsec, 0, 999, 0, 1023));
#endif
      } else if (_mode == LED_FADEINOUT) {
        if (subsec < 1000) {
          if (_level)
#ifdef HALF_BRIGHT
            analogWrite(_pin, map(subsec, 0, 999, 0, 511));
#else
            analogWrite(_pin, map(subsec, 0, 999, 0, 1023));
#endif
          else
#ifdef HALF_BRIGHT
            analogWrite(_pin, map(subsec, 0, 999, 1023, 512));
#else
            analogWrite(_pin, map(subsec, 0, 999, 1023, 0));
#endif
        } else {
          if (_level)
#ifdef HALF_BRIGHT
            analogWrite(_pin, map(subsec, 1000, 1999, 511, 0));
#else
            analogWrite(_pin, map(subsec, 1000, 1999, 1023, 0));
#endif
          else
#ifdef HALF_BRIGHT
            analogWrite(_pin, map(subsec, 1000, 1999, 512, 1023));
#else
            analogWrite(_pin, map(subsec, 1000, 1999, 0, 1023));
#endif
        }
      }
    }
  }
}

void Led::delay(uint32_t ms, uint32_t step) {
  if (_mode <= LED_ON) {
    ::delay(ms);
  } else {
    while (ms >= step) {
      update();
      ::delay(step);
      ms -= step;
    }
    if (ms) {
      update();
      ::delay(ms);
    }
  }
}

void Led::on() {
#ifdef HALF_BRIGHT
  if (_level)
    analogWrite(_pin, 511);
  else
    analogWrite(_pin, 512);
#else
  digitalWrite(_pin, _level);
#endif
  _on = true;
}
