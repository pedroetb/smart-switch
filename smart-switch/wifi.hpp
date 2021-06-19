#ifndef SMART_SWITCH_WIFI_H
#define SMART_SWITCH_WIFI_H

#include <ESP8266WiFi.h>
#include "config.hpp"
#include "common.hpp"

void wifiSetup();
void evalWifiStatus(const uint32_t currEvalTime);
void getWifiMac(char *macBuffer);
void getWifiSsid(char *ssidBuffer);
void getWifiIp(char *ipBuffer);
int32_t getWifiRssi();
bool getWifiStatus();

#endif
