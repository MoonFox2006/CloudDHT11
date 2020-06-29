#ifndef __LIGHTSLEEP_H
#define __LIGHTSLEEP_H

#include <Arduino.h>

typedef void (*sleepcb_t)();

void lightSleep(uint32_t ms, sleepcb_t beforeSleep = NULL, sleepcb_t afterSleep = NULL);

#endif
