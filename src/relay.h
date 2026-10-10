#pragma once

void relay_init();         // starts with the relay OPEN (load disconnected)
void relay_close();        // connect the load
void relay_open();         // disconnect the load
bool relay_isClosed();     // commanded state