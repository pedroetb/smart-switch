#ifndef SMART_SWITCH_MQTT_H
#define SMART_SWITCH_MQTT_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.hpp"
#include "common.hpp"
#include "relay.hpp"
#include "measure.hpp"
#include "timer.hpp"
#include "noise.hpp"
#include "wifi.hpp"
#include "http.hpp"
#include "ota.hpp"

void mqttSetup();
void evalMqttStatus(uint32_t currEvalTime);
void mqttCallback(char *topic, uint8_t *payload, uint8_t length);
void mqttConnect();
void logMqttMessage(const char *message);
void enableMqtt();
void disableMqtt();
bool getMqttEnabled();
bool getMqttStatus();

#endif
