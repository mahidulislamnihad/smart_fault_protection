#include <Arduino.h>
#include "leds.h"
#include "config.h"

static const uint16_t BLINK_HALF_PERIOD_MS = 500;
static const uint16_t COMM_FLASH_MS        = 50;

static LedMode  s_mode      = LED_MODE_OFF;
static uint32_t s_commUntil = 0;
static bool     s_commOn    = false;

static void setLed(uint8_t pin, bool on) {
  digitalWrite(pin, on ? HIGH : LOW);
}

void leds_init() {
  const uint8_t pins[] = { PIN_LED_GREEN, PIN_LED_YELLOW, PIN_LED_RED };
  for (uint8_t i = 0; i < sizeof(pins); i++) {
    digitalWrite(pins[i], LOW);       // off first, then make it an output
    pinMode(pins[i], OUTPUT);
  }
  s_mode = LED_MODE_OFF;
}

void leds_setMode(LedMode mode) {
  s_mode = mode;
}

void leds_commActivity() {
  s_commUntil = millis() + COMM_FLASH_MS;
  s_commOn = true;
}

void leds_update() {
  uint32_t now = millis();
  bool blinkOn = ((now / BLINK_HALF_PERIOD_MS) % 2) == 0;

  setLed(PIN_LED_GREEN, s_mode == LED_MODE_NORMAL);
  setLed(PIN_LED_YELLOW, s_mode == LED_MODE_FAULT);

  bool red = (s_mode == LED_MODE_LOCKOUT) ||
             (s_mode == LED_MODE_LOCKOUT_RESETTABLE && blinkOn);
  setLed(PIN_LED_RED, red);

}