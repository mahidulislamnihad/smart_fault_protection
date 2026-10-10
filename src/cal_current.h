#pragma once

// Current calibration: converts RMS ADC counts to amps.

void  calCurrent_init();
float calCurrent_countsToAmps(float rmsCounts);
bool  calCurrent_setGain(float gain);   // false if gain is out of range
float calCurrent_getGain();