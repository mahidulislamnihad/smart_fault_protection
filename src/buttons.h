#pragma once
#include <stdint.h>

enum ButtonId : uint8_t {
  BTN_UP,
  BTN_DOWN,
  BTN_SELECT,
  BTN_BACK,
  BTN_RESET,
  BUTTON_COUNT
};

void buttons_init();
void buttons_update();                  // call on every loop pass
bool buttons_wasPressed(ButtonId id);   // true once per press, then cleared
bool buttons_isDown(ButtonId id);       // current debounced state