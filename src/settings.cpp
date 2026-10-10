#include <Arduino.h>
#include <math.h>
#include "settings.h"
#include "config.h"

struct Limits { float minV; float maxV; float defV; };

static const Limits LIMITS[SETTING_COUNT] = {
  /* SET_OV_THRESHOLD     */ { 230.0f,   300.0f,   250.0f },
  /* SET_OC_THRESHOLD     */ {   1.0f,    12.0f,    10.0f },
  /* SET_TRIP_DELAY_MS    */ {   0.0f,  1000.0f,    40.0f },
  /* SET_RECONNECT_MS     */ {1000.0f, 60000.0f,  5000.0f },
  /* SET_LOCKOUT_FAULTS   */ {   1.0f, (float)MAX_LOCKOUT_FAULTS, 5.0f },
  /* SET_LOCKOUT_WINDOW_S */ {  10.0f,  3600.0f,   300.0f }
};

static Settings s_cur;

static void apply(SettingId id, float v) {
  switch (id) {
    case SET_OV_THRESHOLD:     s_cur.ovThresholdV      = v;                    break;
    case SET_OC_THRESHOLD:     s_cur.ocThresholdA      = v;                    break;
    case SET_TRIP_DELAY_MS:    s_cur.tripDelayMs       = (uint16_t)(v + 0.5f); break;
    case SET_RECONNECT_MS:     s_cur.reconnectDelayMs  = (uint16_t)(v + 0.5f); break;
    case SET_LOCKOUT_FAULTS:   s_cur.lockoutMaxFaults  = (uint8_t)(v + 0.5f);  break;
    case SET_LOCKOUT_WINDOW_S: s_cur.lockoutWindowSec  = (uint16_t)(v + 0.5f); break;
    default: break;
  }
}

void settings_init() {
  for (uint8_t i = 0; i < SETTING_COUNT; i++) {
    apply((SettingId)i, LIMITS[i].defV);
  }
}

const Settings &settings_get() {
  return s_cur;
}

bool settings_set(SettingId id, float value) {
  if (id >= SETTING_COUNT) return false;
  if (isnan(value)) return false;
  if (value < LIMITS[id].minV || value > LIMITS[id].maxV) return false;
  apply(id, value);
  return true;
}

float settings_getValue(SettingId id) {
  switch (id) {
    case SET_OV_THRESHOLD:     return s_cur.ovThresholdV;
    case SET_OC_THRESHOLD:     return s_cur.ocThresholdA;
    case SET_TRIP_DELAY_MS:    return s_cur.tripDelayMs;
    case SET_RECONNECT_MS:     return s_cur.reconnectDelayMs;
    case SET_LOCKOUT_FAULTS:   return s_cur.lockoutMaxFaults;
    case SET_LOCKOUT_WINDOW_S: return s_cur.lockoutWindowSec;
    default:                   return 0.0f;
  }
}

void settings_getLimits(SettingId id, float &minV, float &maxV, float &defV) {
  if (id >= SETTING_COUNT) { minV = maxV = defV = 0.0f; return; }
  minV = LIMITS[id].minV;
  maxV = LIMITS[id].maxV;
  defV = LIMITS[id].defV;
}