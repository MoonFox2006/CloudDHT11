#include "Config.h"

void *Config::getParamPtr(uint8_t index) {
  switch (index) {
    case 0:
      return wifi_ssid;
    case 1:
      return wifi_pswd;
    case 2:
      return &wifi_wep;
    case 3:
      return ntp_server;
    case 4:
      return &ntp_tz;
    case 5:
      return script_host;
    case 6:
      return &script_port;
    case 7:
      return script_uri;
    case 8:
      return &temp_tolerance;
    case 9:
      return &hum_tolerance;
    case 10:
      return &measure_period;
    case 11:
      return &send_every;
  }
  return NULL;
}
