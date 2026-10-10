#include <Arduino.h>
#include "measurement.h"
#include "adc_sampler.h"
#include "cal_voltage.h"
#include "cal_current.h"

static Measurement s_latest = {0.0f, 0.0f, 0};

// RMS of the AC part of the signal, in ADC counts.
// Subtracting the mean removes the 2.5 V bias automatically.
static float rmsCounts(uint32_t sum, uint32_t sumsq, uint16_t n) {
  float mean = (float)sum / n;
  float var  = (float)sumsq / n - mean * mean;
  return (var > 0.0f) ? sqrt(var) : 0.0f;
}

void measurement_init() {
  calVoltage_init();
  calCurrent_init();
  adcSampler_init();
  s_latest = {0.0f, 0.0f, 0};
}

bool measurement_update() {
  AdcWindow w;
  if (!adcSampler_getWindow(w)) return false;
  if (w.n == 0) return false;                      // guard against divide by zero

  s_latest.vRms = calVoltage_countsToVolts(rmsCounts(w.sumV, w.sumsqV, w.n));
  s_latest.iRms = calCurrent_countsToAmps(rmsCounts(w.sumI, w.sumsqI, w.n));
  s_latest.seq++;
  return true;
}

Measurement measurement_get() {
  return s_latest;
}