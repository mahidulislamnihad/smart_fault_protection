#include <Arduino.h>
#include <string.h>
#include <util/atomic.h>
#include "adc_sampler.h"
#include "config.h"

// Two channels share one ADC, so the timer tick is 2x the per-channel rate.
static const uint32_t TICK_HZ   = 2UL * SAMPLE_RATE_HZ;
static const uint16_t TIMER_TOP = (F_CPU / 8UL / TICK_HZ) - 1;   // 249 for 8 kHz

// Running totals: touched only by the ADC interrupt.
static uint32_t s_sumV, s_sumsqV, s_sumI, s_sumsqI;
static uint16_t s_count;
static uint8_t  s_chan;                 // 0 = voltage, 1 = current

// Published window: written by the interrupt, read by the main loop.
static volatile AdcWindow s_ready;
static volatile bool      s_hasNew;
static volatile uint16_t  s_overruns;

// Timer tick: start one conversion.
ISR(TIMER1_COMPA_vect) {
  ADCSRA |= (1 << ADSC);
}

// Conversion finished: store the result, pick the next channel.
ISR(ADC_vect) {
  uint16_t x  = ADC;
  uint32_t sq = (uint32_t)x * x;

  if (s_chan == 0) {
    s_sumV   += x;
    s_sumsqV += sq;
    s_chan = 1;
  } else {
    s_sumI   += x;
    s_sumsqI += sq;
    s_chan = 0;

    if (++s_count >= SAMPLES_PER_CYCLE) {
      if (s_hasNew) s_overruns++;       // previous window was never read
      s_ready.sumV   = s_sumV;
      s_ready.sumsqV = s_sumsqV;
      s_ready.sumI   = s_sumI;
      s_ready.sumsqI = s_sumsqI;
      s_ready.n      = s_count;
      s_hasNew = true;

      s_sumV = s_sumsqV = s_sumI = s_sumsqI = 0;
      s_count = 0;
    }
  }
  ADMUX = (1 << REFS0) | (s_chan ? ADC_CH_I : ADC_CH_V);
}

void adcSampler_init() {
  s_sumV = s_sumsqV = s_sumI = s_sumsqI = 0;
  s_count = 0;
  s_chan = 0;
  s_hasNew = false;
  s_overruns = 0;

  // ADC: AVcc reference, voltage channel first, clock = 16 MHz / 128 = 125 kHz
  ADMUX = (1 << REFS0) | ADC_CH_V;
#ifdef MUX5
  ADCSRB &= ~(1 << MUX5);               // Mega: select channels 0-7
#endif
  ADCSRA = (1 << ADEN) | (1 << ADIE) |
           (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  DIDR0 |= (1 << ADC0D) | (1 << ADC1D); // turn off digital input buffers on A0, A1

  // Timer1: CTC mode, prescaler 8, interrupt on compare match A
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS11);
  OCR1A  = TIMER_TOP;
  TCNT1  = 0;
  TIMSK1 = (1 << OCIE1A);
}

bool adcSampler_getWindow(AdcWindow &out) {
  bool got = false;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (s_hasNew) {
      memcpy(&out, (const void *)&s_ready, sizeof(AdcWindow));
      s_hasNew = false;
      got = true;
    }
  }
  return got;
}

uint16_t adcSampler_overruns() {
  uint16_t v;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    v = s_overruns;
  }
  return v;
}