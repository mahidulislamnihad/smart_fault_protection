#pragma once
#include <stdint.h>

enum LedMode : uint8_t {
  LED_MODE_OFF,
  LED_MODE_NORMAL,              // green solid
  LED_MODE_FAULT,               // amber solid
  LED_MODE_LOCKOUT,             // red solid
  LED_MODE_LOCKOUT_RESETTABLE   // red blinking
};

void leds_init();
void leds_setMode(LedMode mode);
void leds_commActivity();       // flash the blue LED briefly
void leds_update();             // call from loop()