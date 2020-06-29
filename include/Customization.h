#ifndef __CUSTOMIZATION_H
#define __CUSTOMIZATION_H

//#define USE_SERIAL // Use UART for output
#define USE_LED // Use built-in LED
//#define USE_AUTHORIZATION // Use web page basic authorization

#ifdef USE_AUTHORIZATION
#define AUTH_USER "ESP" // User name for basic authorization
#define AUTH_PSWD "12345678" // Password for basic authorization
#endif

#define CP_SSID "ESP_CloudDHT11" // Non-automatic Captive Portal AP name
#define CP_PSWD "1029384756" // Non-automatic Captive Portal AP password

#define MAX_WIFI_CHANNEL 13 // Max WiFi channel for AP (11 for North America, 13 for Europe)

//#define WIFI_SSID "******" // Your WiFi SSID
//#define WIFI_PSWD "******" // Your WiFi password

#define NTP_SERVER "pool.ntp.org"
#define NTP_TZ 3 // GMT+3

//#define GSCRIPT_URI "" // Your script relative path

#endif
