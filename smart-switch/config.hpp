#ifndef SMART_SWITCH_CONFIG_H
#define SMART_SWITCH_CONFIG_H

#include <Arduino.h>
#include <stdint.h>

// common
constexpr uint32_t bauds = 9600;
constexpr bool serialLogEnabled = true;
constexpr bool mqttLogEnabled = true;
constexpr char rootName[17] = "pedroetb"; // max. length 16
constexpr char commonName[17] = "smart-switch"; // max. length 16
constexpr char uniqueId[4] = "001"; // max. length 3
constexpr uint8_t channelsAvailable = 2;
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

// relay
constexpr uint8_t relayPin[channelsAvailable] = {
	D0, // GPIO16
	D1 // GPIO5
};

// measure
constexpr uint8_t measurePin[channelsAvailable] = {
	D5, // GPIO14
	D6 // GPIO12
};
constexpr uint8_t resetPowerStatusTimeout = 100;
constexpr uint16_t measureFrequencyInterval = 10000;
constexpr uint16_t measureFrequencyDuration = 1000;
constexpr uint8_t measureFrequencyDivisor = 2;

// timer
constexpr uint32_t timerTimeoutDefault = 600000;

// noise
constexpr uint8_t noisePin = D2; // GPIO4
constexpr uint16_t noiseLongDebounceTimeout = 150;
constexpr uint16_t noiseRepetitiveDebounceTimeout = 600;
constexpr uint16_t noisePatternDebounceTimeout = 800;

// wifi
constexpr char wifiSsid[33] = "<ssid>"; // max. length 32
constexpr char wifiPass[64] = "<pass>"; // max. length 63
constexpr uint8_t wifiConnectRetries = 15;
constexpr uint16_t wifiConnectTimeout = 1500;
constexpr uint16_t wifiEvalTimeout = 30000;

// http
constexpr uint16_t httpServerPort = 80;
constexpr uint8_t httpMaxItemLength = 16;
constexpr uint8_t httpRedirectTimeout = 5;
constexpr uint8_t httpRefreshInterval = 30;
constexpr uint16_t httpEvalTimeout = 1000;

// mqtt
constexpr char mqttBrokerUrl[65] = "<broker>"; // max. length 64
constexpr uint16_t mqttBrokerPort = 1883;
constexpr char mqttUser[65] = "<user>"; // max. length 64
constexpr char mqttPass[65] = "<pass>"; // max. length 64
constexpr uint16_t mqttMaxPacketSize = 64; // max. length 28
constexpr uint8_t mqttKeepAlive = 15;
constexpr uint8_t mqttSocketTimeout = 15;
constexpr uint16_t mqttEvalTimeout = 1000;

// ota
constexpr uint16_t otaPort = 8266;
constexpr uint16_t otaEvalTimeout = 3000;

#endif
