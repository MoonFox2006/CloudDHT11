#include <user_interface.h>
#include "LightSleep.h"

const uint32_t SLEEP_CHUNK = 268000; // 268 sec.

static uint32_t sleepRemains;

static void wakeup() {
  if (sleepRemains <= SLEEP_CHUNK) { // Last iteration
    wifi_fpm_close();
  }
}

void lightSleep(uint32_t ms, sleepcb_t beforeSleep, sleepcb_t afterSleep) {
  uint8_t optmode;

  if (beforeSleep)
    beforeSleep();
  wifi_station_disconnect();
  optmode = wifi_get_opmode();
  if (optmode != NULL_MODE)
    wifi_set_opmode_current(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_set_wakeup_cb(wakeup);
  sleepRemains = ms;
  while (sleepRemains > 0) {
    if (sleepRemains > SLEEP_CHUNK)
      ms = SLEEP_CHUNK;
    else
      ms = sleepRemains;
    wifi_fpm_do_sleep(ms * 1000);
    delay(ms);
    sleepRemains -= ms;
  }
  if (optmode != NULL_MODE)
    wifi_set_opmode_current(optmode);
  if (afterSleep)
    afterSleep();
}
