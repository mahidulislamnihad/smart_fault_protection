#pragma once
#include <stdint.h>

struct Settings {
  float    ovThresholdV;
  float    ocThresholdA;
  uint16_t tripDelayMs;
  uint16_t reconnectDelayMs;
  uint8_t  lockoutMaxFaults;
  uint16_t lockoutWindowSec;
};

enum SettingId : uint8_t {
  SET_OV_THRESHOLD,
  SET_OC_THRESHOLD,
  SET_TRIP_DELAY_MS,
  SET_RECONNECT_MS,
  SET_LOCKOUT_FAULTS,
  SET_LOCKOUT_WINDOW_S,
  SETTING_COUNT
};

void            settings_init();                        // load defaults (EEPROM later)
const Settings &settings_get();                         // typed access for logic
bool            settings_set(SettingId id, float value);// false if invalid, nothing changed
float           settings_getValue(SettingId id);        // generic access for UI and PC
void            settings_getLimits(SettingId id, float &minV, float &maxV, float &defV);