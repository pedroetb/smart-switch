#ifndef SMART_SWITCH_COMMON_H
#define SMART_SWITCH_COMMON_H

#include "config.hpp"
#include "relay.hpp"
#include "measure.hpp"
#include "timer.hpp"
#include "noise.hpp"
#include "wifi.hpp"
#include "mqtt.hpp"

void commonSetup();
void restartBoard();
void getDeviceStatus(char *deviceStatusBuffer);
bool validateChannel(const uint8_t channel);
void logSerialMessage(const char *message);
void logMessage(const char *message);

#endif
