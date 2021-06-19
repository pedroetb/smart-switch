#ifndef SMART_SWITCH_OTA_H
#define SMART_SWITCH_OTA_H

#include <ArduinoOTA.h>
#include "config.hpp"
#include "common.hpp"
#include "relay.hpp"
#include "timer.hpp"
#include "noise.hpp"
#include "wifi.hpp"
#include "http.hpp"
#include "mqtt.hpp"

void otaSetup();
void evalOtaStatus(const uint32_t currEvalTime);
void enableOta();
void disableOta();
bool getOtaEnabled();

#endif
