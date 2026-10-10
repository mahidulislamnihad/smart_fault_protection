#pragma once
#include <stdint.h>

struct AdcWindow {
  uint32_t sumV;     // sum of voltage samples (counts)
  uint32_t sumsqV;   // sum of squares of voltage samples
  uint32_t sumI;     // sum of current samples (counts)
  uint32_t sumsqI;   // sum of squares of current samples
  uint16_t n;        // samples per channel in this window
};

void     adcSampler_init();                     // set up ADC and timer, start sampling
bool     adcSampler_getWindow(AdcWindow &out);  // true if a new window was ready
uint16_t adcSampler_overruns();                 // windows lost because main was too slow