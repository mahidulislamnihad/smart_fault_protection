#include "cal_voltage.h"
#include "config.h"

// Allowed gain range: +/-20 % around the default.
static const float GAIN_MIN = K_VOLTS_PER_COUNT_DEFAULT * 0.8f;
static const float GAIN_MAX = K_VOLTS_PER_COUNT_DEFAULT * 1.2f;

static float s_gain = K_VOLTS_PER_COUNT_DEFAULT;

void calVoltage_init() {
  s_gain = K_VOLTS_PER_COUNT_DEFAULT;   // later: load from EEPROM
}

float calVoltage_countsToVolts(float rmsCounts) {
  return rmsCounts * s_gain;
}

bool calVoltage_setGain(float gain) {
  if (gain < GAIN_MIN || gain > GAIN_MAX) return false;
  s_gain = gain;
  return true;
}

float calVoltage_getGain() {
  return s_gain;
}