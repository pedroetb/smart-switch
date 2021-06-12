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
constexpr uint8_t httpTimeoutsLength = 10;
constexpr uint8_t httpTimeoutLabelMaxSize = 4;
constexpr uint8_t httpTimeoutValueMaxSize = 11;
constexpr char httpTimeoutLabels[httpTimeoutsLength][httpTimeoutLabelMaxSize] = {
	"1m", "5m", "10m", "30m", "1h",
	"6h", "12h", "1d", "1w", "1mo"
};
constexpr char httpTimeoutValues[httpTimeoutsLength][httpTimeoutValueMaxSize] = {
	"60000", "300000", "600000", "1800000", "3600000",
	"21600000", "43200000", "86400000", "604800000", "2628000000"
};

void httpSetup();
void evalHttpStatus(uint32_t currEvalTime);
void enableHttp();
void disableHttp();
bool getHttpEnabled();

#endif
