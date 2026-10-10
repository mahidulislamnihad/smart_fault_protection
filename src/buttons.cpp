#include <Arduino.h>
#include "buttons.h"
#include "config.h"

static const uint8_t PINS[BUTTON_COUNT] = {
  PIN_BTN_UP, PIN_BTN_DOWN, PIN_BTN_SELECT, PIN_BTN_BACK, PIN_BTN_RESET
};

static bool     s_stable[BUTTON_COUNT];        // debounced state, true = pressed
static bool     s_lastRaw[BUTTON_COUNT];       // last raw reading
static uint32_t s_changedAt[BUTTON_COUNT];     // when the raw reading last changed
static bool     s_pressedEvent[BUTTON_COUNT];  // set on press, cleared when read

void buttons_init() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    pinMode(PINS[i], INPUT_PULLUP);
    s_stable[i] = false;
    s_lastRaw[i] = false;
    s_changedAt[i] = 0;
    s_pressedEvent[i] = false;
  }
}

void buttons_update() {
  uint32_t now = millis();
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    bool raw = (digitalRead(PINS[i]) == LOW);        // pressed = LOW

    if (raw != s_lastRaw[i]) {
      s_lastRaw[i] = raw;                            // changed: restart the timer
      s_changedAt[i] = now;
    } else if (raw != s_stable[i] && (now - s_changedAt[i]) >= BUTTON_DEBOUNCE_MS) {
      s_stable[i] = raw;                             // steady long enough: accept
      if (raw) s_pressedEvent[i] = true;
    }
  }
}

bool buttons_wasPressed(ButtonId id) {
  if (id >= BUTTON_COUNT) return false;
  bool e = s_pressedEvent[id];
  s_pressedEvent[id] = false;
  return e;
}

bool buttons_isDown(ButtonId id) {
  return (id < BUTTON_COUNT) && s_stable[id];
}