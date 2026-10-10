#include "cal_current.h"
#include "config.h"

// Allowed gain range: +/-20 % around the default.
static const float GAIN_MIN = K_AMPS_PER_COUNT_DEFAULT * 0.8f;
static const float GAIN_MAX = K_AMPS_PER_COUNT_DEFAULT * 1.2f;

static float s_gain = K_AMPS_PER_COUNT_DEFAULT;

void calCurrent_init() {
  s_gain = K_AMPS_PER_COUNT_DEFAULT;   // later: load from EEPROM
}

float calCurrent_countsToAmps(float rmsCounts) {
  return rmsCounts * s_gain;
}

bool calCurrent_setGain(float gain) {
  if (gain < GAIN_MIN || gain > GAIN_MAX) return false;
  s_gain = gain;
  return true;
}

float calCurrent_getGain() {
  return s_gain;
}