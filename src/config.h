#pragma once 

#define FW_VERSION "0.1"
//PINS
#define PIN_V_SENSE A0
#define PIN_I_SENSE A1

//sampleing
#define SAMPLE_RATE_HZ 4000
#define SAMPLES_PER_CYCLE 80

// calibration constant
#define K_VOLTS_PER_COUNT_DEFAULT   0.9421f
#define K_AMPS_PER_COUNT_DEFAULT    0.0739f

//ADC Channel
#define ADC_CH_V   0   // A0 = ADC channel 0
#define ADC_CH_I   1   // A1 = ADC channel 1

//relay
#define PIN_RELAY          6
#define RELAY_ACTIVE_HIGH  1    // 1 = pin HIGH energizes the coil, 0 = pin LOW does

//led pins
#define PIN_LED_GREEN   22
#define PIN_LED_YELLOW   23
#define PIN_LED_RED     24


//protection

#define WINDOW_MS            (1000UL * SAMPLES_PER_CYCLE / SAMPLE_RATE_HZ)  // 20 ms per measurement
#define SEVERE_FACTOR_V      1.15f   // voltage this far above the limit trips at once
#define SEVERE_FACTOR_I      2.0f    // current this far above the limit trips at once
#define HYST_V               5.0f    // voltage must fall this far below the limit to count as safe
#define MAX_LOCKOUT_FAULTS   20      // size of the fault history

//Buttons
#define PIN_BTN_UP       26
#define PIN_BTN_DOWN     27
#define PIN_BTN_SELECT   28
#define PIN_BTN_BACK     29
#define PIN_BTN_RESET    30   //  reset button for lockout

#define BUTTON_DEBOUNCE_MS  30

//LCD
#define PIN_LCD_RS   12
#define PIN_LCD_EN   11
#define PIN_LCD_D4   10
#define PIN_LCD_D5   9
#define PIN_LCD_D6   8
#define PIN_LCD_D7   7
#define LCD_COLS     16
#define LCD_ROWS     2

#define UI_TIMEOUT_MS  5000UL   // menu returns to the status screen after 5 s idle