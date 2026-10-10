#include <Arduino.h>
#include "protection.h"
#include "settings.h"
#include "relay.h"
#include "config.h"

static ProtState s_state     = PROT_WAIT_RECONNECT;
static FaultType s_lastFault = FAULT_NONE;

// Latest voltage knowledge
static bool     s_haveMeas = false;
static bool     s_vSafe    = false;

// "Voltage has been safe since..." timer, used while the relay is open
static bool     s_safeTimerRunning = false;
static uint32_t s_safeSince        = 0;

// Consecutive over-limit measurements (mild-fault persistence)
static uint8_t  s_ovCount = 0;
static uint8_t  s_ocCount = 0;

// Fault history ring buffer
static uint32_t s_faultTime[MAX_LOCKOUT_FAULTS];
static bool     s_faultValid[MAX_LOCKOUT_FAULTS];
static uint8_t  s_faultHead = 0;

// ---------- fault history ----------

static void pruneFaults(uint32_t now) {
  uint32_t windowMs = 1000UL * settings_get().lockoutWindowSec;
  for (uint8_t i = 0; i < MAX_LOCKOUT_FAULTS; i++) {
    if (s_faultValid[i] && (now - s_faultTime[i]) > windowMs) s_faultValid[i] = false;
  }
}

static void recordFault(uint32_t now) {
  s_faultTime[s_faultHead]  = now;
  s_faultValid[s_faultHead] = true;
  s_faultHead = (s_faultHead + 1) % MAX_LOCKOUT_FAULTS;
}

static uint8_t recentFaults() {
  uint8_t n = 0;
  for (uint8_t i = 0; i < MAX_LOCKOUT_FAULTS; i++) if (s_faultValid[i]) n++;
  return n;
}

static void clearFaults() {
  for (uint8_t i = 0; i < MAX_LOCKOUT_FAULTS; i++) s_faultValid[i] = false;
}

// ---------- helpers ----------

static uint8_t tripWindows(const Settings &s) {
  uint8_t w = (uint8_t)((s.tripDelayMs + WINDOW_MS - 1) / WINDOW_MS);
  return (w < 1) ? 1 : w;
}

static void trip(FaultType f, uint32_t now) {
  relay_open();                       // first: protect the appliance
  s_lastFault = f;
  s_ovCount = 0;
  s_ocCount = 0;
  s_safeTimerRunning = false;

  pruneFaults(now);
  recordFault(now);

  if (recentFaults() >= settings_get().lockoutMaxFaults) s_state = PROT_LOCKOUT;
  else                                                   s_state = PROT_WAIT_RECONNECT;
}

// ---------- public interface ----------

void protection_init() {
  relay_open();
  s_state     = PROT_WAIT_RECONNECT;
  s_lastFault = FAULT_NONE;
  s_haveMeas  = false;
  s_vSafe     = false;
  s_safeTimerRunning = false;
  s_ovCount = s_ocCount = 0;
  s_faultHead = 0;
  clearFaults();
}

void protection_onMeasurement(const Measurement &m) {
  const Settings &s = settings_get();
  uint32_t now = millis();

  s_haveMeas = true;
  s_vSafe = (m.vRms < s.ovThresholdV - HYST_V);

  if (s_state == PROT_CONNECTED) {
    bool severeV = (m.vRms >= s.ovThresholdV * SEVERE_FACTOR_V);
    bool severeI = (m.iRms >= s.ocThresholdA * SEVERE_FACTOR_I);
    bool overV   = (m.vRms >  s.ovThresholdV);
    bool overI   = (m.iRms >  s.ocThresholdA);

    s_ovCount = overV ? (uint8_t)(s_ovCount + 1) : 0;
    s_ocCount = overI ? (uint8_t)(s_ocCount + 1) : 0;
    uint8_t need = tripWindows(s);

    if (severeV || s_ovCount >= need)      trip(FAULT_OVERVOLTAGE, now);
    else if (severeI || s_ocCount >= need) trip(FAULT_OVERCURRENT, now);
  } else {
    // Relay is open: measure how long the voltage has stayed safe
    if (s_vSafe) {
      if (!s_safeTimerRunning) { s_safeTimerRunning = true; s_safeSince = now; }
    } else {
      s_safeTimerRunning = false;
    }
  }
}

void protection_update() {
  uint32_t now = millis();
  pruneFaults(now);

  if (s_state == PROT_WAIT_RECONNECT && s_safeTimerRunning &&
      (now - s_safeSince) >= settings_get().reconnectDelayMs) {
    relay_close();
    s_state = PROT_CONNECTED;
    s_ovCount = s_ocCount = 0;
    s_safeTimerRunning = false;
  }
}

bool protection_isResetAllowed() {
  return (s_state == PROT_LOCKOUT) && s_haveMeas && s_vSafe;
}

bool protection_requestReset() {
  if (!protection_isResetAllowed()) return false;
  clearFaults();
  s_state = PROT_WAIT_RECONNECT;       // still waits out the reconnect delay
  s_safeTimerRunning = true;
  s_safeSince = millis();
  return true;
}

ProtState protection_getState()     { return s_state; }
FaultType protection_getLastFault() { return s_lastFault; }
uint8_t   protection_faultCount()   { return recentFaults(); }