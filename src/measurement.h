#pragma once
#include <stdint.h>

struct Measurement {
  float    vRms;   // mains voltage, volts RMS
  float    iRms;   // load current, amps RMS
  uint32_t seq;    // increases by 1 with every new measurement
};

void        measurement_init();
bool        measurement_update();   // call from loop(); true if a new result was produced
Measurement measurement_get();      // latest result