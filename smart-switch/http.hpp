#ifndef SMART_SWITCH_HTTP_H
#define SMART_SWITCH_HTTP_H

#include <ESP8266WebServer.h>
#include "config.hpp"
#include "common.hpp"
#include "relay.hpp"
#include "measure.hpp"
#include "timer.hpp"
#include "noise.hpp"
#include "wifi.hpp"
#include "mqtt.hpp"
#include "ota.hpp"

constexpr uint8_t httpMaxItemLength = 16; // Request path, arg name and arg value

void httpSetup();
void evalHttpStatus(uint32_t currEvalTime);
void enableHttp();
void disableHttp();
bool getHttpEnabled();

#endif
