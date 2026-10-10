#pragma once
#include <stdint.h>
#include "measurement.h"

enum ProtState : uint8_t {
  PROT_WAIT_RECONNECT,   // relay open, waiting for safe conditions
  PROT_CONNECTED,        // relay closed, monitoring
  PROT_LOCKOUT           // relay open until the user resets
};

enum FaultType : uint8_t {
  FAULT_NONE,
  FAULT_OVERVOLTAGE,
  FAULT_OVERCURRENT
};

void      protection_init();
void      protection_onMeasurement(const Measurement &m);  // call for every new measurement
void      protection_update();                             // call on every loop pass
bool      protection_requestReset();                       // true if the reset was accepted
bool      protection_isResetAllowed();                     // lockout and voltage safe
ProtState protection_getState();
FaultType protection_getLastFault();
uint8_t   protection_faultCount();                         // faults inside the lockout window