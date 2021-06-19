#ifndef SMART_SWITCH_RELAY_H
#define SMART_SWITCH_RELAY_H

#include "config.hpp"
#include "common.hpp"
#include "measure.hpp"

void relaySetup();
bool getRelayStatus(const uint8_t index);
void enableRelay(const uint8_t index);
void disableRelay(const uint8_t index);
void toggleRelay(const uint8_t index);
void toggleRelay();
void switchOn(const uint8_t index);
void switchOn();
void switchOff(const uint8_t index);
void switchOff();

#endif
