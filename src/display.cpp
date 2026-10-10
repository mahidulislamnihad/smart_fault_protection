#include <Arduino.h>
#include <LiquidCrystal.h>
#include <string.h>
#include "display.h"
#include "config.h"

static LiquidCrystal s_lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);
static char s_cache[LCD_ROWS][LCD_COLS + 1];

void display_init() {
  s_lcd.begin(LCD_COLS, LCD_ROWS);
  s_lcd.clear();
  for (uint8_t r = 0; r < LCD_ROWS; r++) {
    memset(s_cache[r], ' ', LCD_COLS);
    s_cache[r][LCD_COLS] = '\0';
  }
}

void display_setLine(uint8_t row, const char *text) {
  if (row >= LCD_ROWS) return;

  char line[LCD_COLS + 1];
  uint8_t i = 0;
  for (; i < LCD_COLS && text[i]; i++) line[i] = text[i];
  for (; i < LCD_COLS; i++)            line[i] = ' ';
  line[LCD_COLS] = '\0';

  if (strcmp(line, s_cache[row]) == 0) return;      // unchanged: skip the slow write

  s_lcd.setCursor(0, row);
  s_lcd.print(line);
  memcpy(s_cache[row], line, LCD_COLS + 1);
}