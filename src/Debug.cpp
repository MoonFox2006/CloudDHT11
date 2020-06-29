#include <Arduino.h>
#include "Debug.h"

void debug_log(char type, const char *fmt, ...) {
  char msg[256];
  va_list vargs;

  va_start(vargs, fmt);
  if (type) {
    msg[0] = '[';
    msg[1] = type;
    msg[2] = ']';
    msg[3] = ' ';
    vsnprintf_P(&msg[4], sizeof(msg) - 4, fmt, vargs);
  } else {
    vsnprintf_P(msg, sizeof(msg), fmt, vargs);
  }
  va_end(vargs);
  if (strlen(msg) == sizeof(msg) - 1) {
    for (uint16_t i = sizeof(msg) - 4; i < sizeof(msg) - 1; ++i)
      msg[i] = '.';
  }
  Serial.println(msg);
  Serial.flush();
}
