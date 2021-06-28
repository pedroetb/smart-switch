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
constexpr uint8_t channelsAvailable = 2; // if changed, add new pins and one attach per channel at measure setup

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

// timer
constexpr bool timerEnabledDefault = true;
constexpr uint32_t timerTimeoutDefault = 600000;
constexpr uint16_t timerWarmingTimeout = resetPowerStatusTimeout * 1.5;

// noise
constexpr bool noiseEnabledDefault = true;
constexpr uint8_t noisePin = D2; // GPIO4
constexpr uint16_t noiseLongDebounceTimeout = 150;
constexpr uint16_t noiseRepetitiveDebounceTimeout = 600;
constexpr uint16_t noisePatternDebounceTimeout = 800;

// wifi
constexpr char wifiSsid[33] = "<ssid>"; // max. length 32
constexpr char wifiPass[64] = "<pass>"; // max. length 63
constexpr uint16_t wifiEvalInterval = 10000;

// http
constexpr bool httpEnabledDefault = true;
constexpr uint16_t httpServerPort = 80;
constexpr uint8_t httpRedirectTimeout = 5;
constexpr uint8_t httpRefreshInterval = 30;
constexpr uint16_t httpEvalInterval = 1000;

// mqtt
constexpr bool mqttEnabledDefault = true;
constexpr char mqttBrokerUrl[65] = "<broker>"; // max. length 64
constexpr uint16_t mqttBrokerPort = 1883;
constexpr char mqttUser[65] = "<user>"; // max. length 64
constexpr char mqttPass[65] = "<pass>"; // max. length 64
constexpr uint16_t mqttEvalInterval = 1000;
constexpr uint16_t mqttReconnectInterval = 10000;

// ota
constexpr uint16_t otaPort = 8266;
constexpr uint16_t otaEvalInterval = 3000;

#endif
