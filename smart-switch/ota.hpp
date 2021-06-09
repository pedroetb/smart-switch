#ifndef SMART_SWITCH_OTA_H
#define SMART_SWITCH_OTA_H

#include <ArduinoOTA.h>
#include "config.hpp"
#include "common.hpp"
#include "relay.hpp"
#include "wifi.hpp"
#include "mqtt.hpp"

void otaSetup();
void evalOtaStatus(uint32_t currEvalTime);
void enableOta();
void disableOta();
bool getOtaEnabled();

#endif
