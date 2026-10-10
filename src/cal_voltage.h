#pragma once

// Voltage calibration: converts RMS ADC counts to mains volts.

void  calVoltage_init();                        // load the default gain
float calVoltage_countsToVolts(float rmsCounts);
bool  calVoltage_setGain(float gain);           // false if gain is out of range
float calVoltage_getGain();