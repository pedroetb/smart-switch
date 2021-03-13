#ifndef SMART_SWITCH_COMMON_H
#define SMART_SWITCH_COMMON_H

#include "config.hpp"
#include "relay.hpp"
#include "timer.hpp"
#include "noise.hpp"
#include "wifi.hpp"
#include "mqtt.hpp"

void commonSetup();
void restartBoard();
String getDeviceStatus();
void logSerialMessage(String message);
void logMessage(String message);

#endif
