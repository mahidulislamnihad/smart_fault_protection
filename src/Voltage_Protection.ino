#include "config.h"
#include "measurement.h"
#include "relay.h"
#include "leds.h"
#include "buttons.h"
#include "settings.h"
#include "protection.h"
#include "display.h"
#include "ui.h"

static const char *stateName(ProtState s) {
  switch (s) {
    case PROT_CONNECTED:      return "CONNECTED";
    case PROT_WAIT_RECONNECT: return "WAIT_RECONNECT";
    case PROT_LOCKOUT:        return "LOCKOUT";
  }
  return "?";
}

static const char *faultName(FaultType f) {
  switch (f) {
    case FAULT_OVERVOLTAGE: return "OVERVOLTAGE";
    case FAULT_OVERCURRENT: return "OVERCURRENT";
    default:                return "none";
  }
}

static void showStatusOnLeds() {
  switch (protection_getState()) {
    case PROT_CONNECTED:      leds_setMode(LED_MODE_NORMAL); break;
    case PROT_WAIT_RECONNECT: leds_setMode(LED_MODE_FAULT);  break;
    case PROT_LOCKOUT:
      leds_setMode(protection_isResetAllowed() ? LED_MODE_LOCKOUT_RESETTABLE
                                               : LED_MODE_LOCKOUT);
      break;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.print("Voltage Protector v");
  Serial.println(FW_VERSION);

  settings_init();
  relay_init();
  leds_init();
  buttons_init();
  measurement_init();
  protection_init();
  display_init();
  ui_init();
}

void loop() {
  static uint8_t printCount = 0;

  buttons_update();

  if (measurement_update()) {
    Measurement m = measurement_get();
    protection_onMeasurement(m);

    if (++printCount >= 25) {                // about twice per second
      printCount = 0;
      Serial.print(stateName(protection_getState()));
      Serial.print("  V=");      Serial.print(m.vRms, 1);
      Serial.print(" I=");       Serial.print(m.iRms, 2);
      Serial.print("  relay=");  Serial.print(relay_isClosed() ? "ON" : "OFF");
      Serial.print("  last=");   Serial.print(faultName(protection_getLastFault()));
      Serial.print(" faults=");  Serial.println(protection_faultCount());
    }
  }

  protection_update();

  if (buttons_wasPressed(BTN_RESET)) {
    Serial.println(protection_requestReset() ? ">> reset accepted" : ">> reset refused");
  }

  showStatusOnLeds();
  leds_update();
  ui_update();
}