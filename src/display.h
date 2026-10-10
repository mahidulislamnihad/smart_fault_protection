#pragma once
#include <stdint.h>

void display_init();
void display_setLine(uint8_t row, const char *text);   // pads to 16 chars, writes only if changed