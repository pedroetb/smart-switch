#ifndef SMART_SWITCH_COMMON_H
#define SMART_SWITCH_COMMON_H

#include "config.hpp"
#include "relay.hpp"
#include "measure.hpp"
#include "timer.hpp"
#include "noise.hpp"
#include "wifi.hpp"
#include "mqtt.hpp"

constexpr uint8_t actionsLength = 17;
constexpr uint8_t maxActionSize = 15; // max. length 14
constexpr char actions[actionsLength][maxActionSize] = {
	"/", "/status", "/reset",
	"/on", "/off", "/toggle",
	"/enable-noise", "/disable-noise",
	"/enable-timer", "/disable-timer", "/set-timer",
	"/enable-http", "/disable-http",
	"/enable-mqtt", "/disable-mqtt",
	"/enable-ota", "/disable-ota",
};
constexpr uint8_t arrayValueSize = 3 * channelsAvailable + 3;
constexpr uint16_t deviceStatusMaxSize = 384 + (3 * channelsAvailable + 2) * 4;
constexpr char deviceStatusTemplate[430] = "{\n"
	"\t\"powerStatus\": %s,\n"
	"\t\"netFrequency\": %s,\n"
	"\t\"relayStatus\": %s,\n"
	"\t\"timerEnabled\": %u,\n"
	"\t\"timerElapsedTime\": %u,\n"
	"\t\"timerTimeout\": %u,\n"
	"\t\"noiseEnabled\": %s,\n"
	"\t\"noiseValue\": %u,\n"
	"\t\"wifiStatus\": %u,\n"
	"\t\"mac\": \"%s\",\n"
	"\t\"ssid\": \"%s\",\n"
	"\t\"ip\": \"%s\",\n"
	"\t\"rssi\": %i,\n"
	"\t\"httpEnabled\": %u,\n"
	"\t\"mqttEnabled\": %u,\n"
	"\t\"mqttStatus\": %u,\n"
	"\t\"otaEnabled\": %u\n"
	"}";

void commonSetup();
void restartBoard();
void getDeviceStatus(char *deviceStatusBuffer);
bool validateChannel(const uint8_t channel);
void logSerialMessage(const char *message);
void logMessage(const char *message);

#endif
