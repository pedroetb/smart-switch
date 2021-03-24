#ifndef SMART_SWITCH_WIFI_H
#define SMART_SWITCH_WIFI_H

#include <ESP8266WiFi.h>
#include "config.hpp"
#include "common.hpp"

void wifiSetup();
void evalWifiStatus(uint32_t currEvalTime);
String getWifiMac();
String getWifiSsid();
String getWifiIp();
int32_t getWifiRssi();
bool getWifiStatus();

#endif
