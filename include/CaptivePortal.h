#ifndef __CAPTIVEPORTAL_H
#define __CAPTIVEPORTAL_H

#include <Print.h>
#include <DNSServer.h>
#include "BaseWebServer.h"
#ifdef USE_LED
#include "Led.h"
#endif

class CaptivePortal : public BaseWebServer {
public:
#ifdef USE_LED
  CaptivePortal(const BaseConfig *config, const Led &led, const Print *out = NULL) : BaseWebServer(config), _out((Print*)out), _led((Led&)led), _dns(NULL) {}
#else
  CaptivePortal(const BaseConfig *config, const Print *out = NULL) : BaseWebServer(config), _out((Print*)out), _dns(NULL) {}
#endif
  virtual ~CaptivePortal() {
    if (_dns)
      delete[] _dns;
  }

  bool _setup();
  void _loop();

  virtual bool exec(uint16_t duration = 45);

  virtual String ssid() const;
  virtual String password() const;
  virtual uint8_t channel() const;

protected:
#ifdef USE_LED
  static const Led::ledmode_t LED_CPWAITING = Led::LED_2HZ;
  static const Led::ledmode_t LED_CPPROCESSING = Led::LED_1HZ;
#endif

  void handleRoot() {
    handleSetup();
  }
  bool beforeHandle() {
    return (! isCaptivePortal());
  }

  virtual bool isCaptivePortal();

  Print *_out;
#ifdef USE_LED
  Led &_led;
#endif
  DNSServer *_dns;
};

#endif
