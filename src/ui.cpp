#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include "ui.h"
#include "display.h"
#include "buttons.h"
#include "measurement.h"
#include "protection.h"
#include "settings.h"
#include "config.h"

enum UiScreen : uint8_t { SCR_STATUS, SCR_BROWSE, SCR_EDIT };

struct Meta {
  const char *name;
  const char *unit;
  float       step;
  uint8_t     decimals;
};

static const Meta META[SETTING_COUNT] = {
  { "OV limit",        "V",  1.0f,   0 },
  { "OC limit",        "A",  0.5f,   1 },
  { "Trip delay",      "ms", 10.0f,  0 },
  { "Reconnect delay", "ms", 500.0f, 0 },
  { "Lockout faults",  "",   1.0f,   0 },
  { "Lockout window",  "s",  10.0f,  0 }
};

static UiScreen s_screen    = SCR_STATUS;
static uint8_t  s_index     = 0;
static float    s_editValue = 0.0f;
static uint32_t s_lastInput = 0;
static uint32_t s_lastDraw  = 0;

static void formatValue(uint8_t idx, float v, char *out, size_t n) {
  char num[12];
  if (META[idx].decimals > 0) dtostrf(v, 1, META[idx].decimals, num);
  else                        snprintf(num, sizeof(num), "%ld", (long)(v + 0.5f));
  snprintf(out, n, "%s%s%s", num, META[idx].unit[0] ? " " : "", META[idx].unit);
}

static void drawStatus() {
  Measurement m = measurement_get();
  const Settings &s = settings_get();
  char vbuf[10], ibuf[10], l0[24], l1[24];

  dtostrf(m.vRms, 5, 1, vbuf);
  dtostrf(m.iRms, 4, 1, ibuf);
  snprintf(l0, sizeof(l0), "V=%sV I=%sA", vbuf, ibuf);

  switch (protection_getState()) {
    case PROT_CONNECTED:
      snprintf(l1, sizeof(l1), "LOAD ON  F:%u/%u",
               (unsigned)protection_faultCount(), (unsigned)s.lockoutMaxFaults);
      break;
    case PROT_WAIT_RECONNECT: {
      FaultType f = protection_getLastFault();
      const char *tag = (f == FAULT_OVERVOLTAGE) ? "OV" :
                        (f == FAULT_OVERCURRENT) ? "OC" : "--";
      snprintf(l1, sizeof(l1), "WAIT %s  F:%u/%u", tag,
               (unsigned)protection_faultCount(), (unsigned)s.lockoutMaxFaults);
      break;
    }
    case PROT_LOCKOUT:
    default:
      snprintf(l1, sizeof(l1), "%s",
               protection_isResetAllowed() ? "LOCKED-PRESS RST" : "LOCKED-WAIT SAFE");
      break;
  }
  display_setLine(0, l0);
  display_setLine(1, l1);
}

static void drawSetting(bool editing) {
  char val[20], l1[24];
  float v = editing ? s_editValue : settings_getValue((SettingId)s_index);
  formatValue(s_index, v, val, sizeof(val));
  snprintf(l1, sizeof(l1), "%s%s", editing ? "> " : "", val);
  display_setLine(0, META[s_index].name);
  display_setLine(1, l1);
}

void ui_init() {
  s_screen = SCR_STATUS;
  s_lastInput = millis();
  s_lastDraw = 0;
}

void ui_update() {
  uint32_t now = millis();

  // Read every button each pass so no press is left over for later
  bool up   = buttons_wasPressed(BTN_UP);
  bool down = buttons_wasPressed(BTN_DOWN);
  bool sel  = buttons_wasPressed(BTN_SELECT);
  bool back = buttons_wasPressed(BTN_BACK);
  bool any  = up || down || sel || back;
  if (any) s_lastInput = now;

  // Idle timeout: back to the status screen, cancelling any edit
  if (s_screen != SCR_STATUS && (now - s_lastInput) > UI_TIMEOUT_MS) {
    s_screen = SCR_STATUS;
    any = true;                       // force a redraw
  }

  switch (s_screen) {
    case SCR_STATUS:
      if (sel) { s_screen = SCR_BROWSE; s_index = 0; }
      break;

    case SCR_BROWSE:
      if (up)   s_index = (s_index + SETTING_COUNT - 1) % SETTING_COUNT;
      if (down) s_index = (s_index + 1) % SETTING_COUNT;
      if (sel)  { s_editValue = settings_getValue((SettingId)s_index); s_screen = SCR_EDIT; }
      if (back) s_screen = SCR_STATUS;
      break;

    case SCR_EDIT: {
      float minV, maxV, defV;
      settings_getLimits((SettingId)s_index, minV, maxV, defV);
      if (up)   { s_editValue += META[s_index].step; if (s_editValue > maxV) s_editValue = maxV; }
      if (down) { s_editValue -= META[s_index].step; if (s_editValue < minV) s_editValue = minV; }
      if (sel)  { if (settings_set((SettingId)s_index, s_editValue)) s_screen = SCR_BROWSE; }
      if (back) s_screen = SCR_BROWSE;       // cancel, nothing saved
      break;
    }
  }

  // Redraw on input, otherwise at most 5 times per second
  if (any || (now - s_lastDraw) >= 200) {
    s_lastDraw = now;
    if (s_screen == SCR_STATUS)      drawStatus();
    else if (s_screen == SCR_BROWSE) drawSetting(false);
    else                             drawSetting(true);
  }
}