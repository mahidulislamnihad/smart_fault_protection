#include <Arduino.h>
#include "relay.h"
#include "config.h"

static bool s_closed = false;

static void drive(bool close) {
  bool level = RELAY_ACTIVE_HIGH ? close : !close;
  digitalWrite(PIN_RELAY, level ? HIGH : LOW);
  s_closed = close;
}

void relay_init() {
  // Write the "open" level BEFORE making the pin an output,
  // so the relay never twitches on for an instant at start-up.
  bool openLevel = RELAY_ACTIVE_HIGH ? LOW : HIGH;
  digitalWrite(PIN_RELAY, openLevel);
  pinMode(PIN_RELAY, OUTPUT);
  s_closed = false;
}

void relay_close()     { drive(true);  }
void relay_open()      { drive(false); }
bool relay_isClosed()  { return s_closed; }