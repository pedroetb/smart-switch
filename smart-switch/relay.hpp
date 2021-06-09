#ifndef SMART_SWITCH_RELAY_H
#define SMART_SWITCH_RELAY_H

#include "config.hpp"
#include "common.hpp"
#include "measure.hpp"

void relaySetup();
bool getRelayStatus(uint8_t index);
void enableRelay(uint8_t index);
void disableRelay(uint8_t index);
void toggleRelay(uint8_t index);
void toggleRelay();
void switchOn(uint8_t index);
void switchOn();
void switchOff(uint8_t index);
void switchOff();

#endif
