#ifndef SMART_SWITCH_CONFIG_H
#define SMART_SWITCH_CONFIG_H

#include <Arduino.h>
#include <stdint.h>

// common
constexpr uint32_t bauds = 9600;
constexpr bool serialLogEnabled = true;
constexpr bool mqttLogEnabled = true;
constexpr char* rootName = "pedroetb";
constexpr char* commonName = "smart-switch";
constexpr char* uniqueId = "001";
constexpr uint8_t actionsLength = 15;
constexpr char* actions[actionsLength] = {
  "/", "/status", "/reset",
  "/on", "/off", "/toggle",
  "/enable-noise", "/disable-noise",
  "/enable-timer", "/disable-timer", "/set-timer",
  "/enable-http", "/disable-http",
  "/enable-mqtt", "/disable-mqtt",
};

// relay
constexpr uint8_t relayPin = 5;

// timer
constexpr uint32_t timerTimeoutDefault = 600000;

// noise
constexpr uint8_t noisePin = 4;
constexpr uint16_t noiseLongDebounceTimeout = 150;
constexpr uint16_t noiseRepetitiveDebounceTimeout = 600;
constexpr uint16_t noisePatternDebounceTimeout = 800;

// wifi
constexpr char* wifiSsid = "<ssid>";
constexpr char* wifiPass = "<pass>";
constexpr uint8_t wifiConnectRetries = 15;
constexpr uint16_t wifiConnectTimeout = 1500;
constexpr uint16_t wifiEvalTimeout = 30000;

// http
constexpr uint16_t httpServerPort = 80;
constexpr uint8_t httpRedirectTimeout = 5;
constexpr uint8_t httpRefreshInterval = 30;
constexpr uint16_t httpEvalTimeout = 1000;

// mqtt
constexpr char* mqttBrokerUrl = "<broker>";
constexpr uint16_t mqttBrokerPort = 1883;
constexpr char* mqttUser = "<user>";
constexpr char* mqttPass = "<pass>";
constexpr uint16_t mqttMaxPacketSize = 256;
constexpr uint8_t mqttKeepAlive = 15;
constexpr uint8_t mqttSocketTimeout = 15;
constexpr uint16_t mqttEvalTimeout = 1000;

#endif
