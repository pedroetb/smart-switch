#ifndef SMART_SWITCH_RELAY_H
#define SMART_SWITCH_RELAY_H

#include "config.hpp"
#include "common.hpp"
#include "measure.hpp"

void relaySetup();
bool getRelayStatus();
void enableRelay();
void disableRelay();
void toggleRelay();
void switchOn();
void switchOff();

#endif
