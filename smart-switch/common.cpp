#include "common.hpp"

void commonSetup() {

  if (serialLogEnabled) {
    Serial.begin(bauds);
  }
  logSerialMessage("\n--- Start smart-switch ---");
}

void restartBoard() {

  delay(500);
  ESP.restart();
}

String getDeviceStatus() {

  char deviceStatusBuffer[400];

  sprintf(deviceStatusBuffer,
    "{\n"
      "\t\"relayStatus\": %u,\n"
      "\t\"measurePowerStatus\": %u,\n"
      "\t\"measureFrequency\": %u,\n"
      "\t\"noiseEnabled\": %u,\n"
      "\t\"noiseValue\": %u,\n"
      "\t\"timerEnabled\": %u,\n"
      "\t\"timerElapsedTime\": %u,\n"
      "\t\"timerTimeout\": %u,\n"
      "\t\"wifiStatus\": %u,\n"
      "\t\"mac\": \"%s\",\n"
      "\t\"ssid\": \"%s\",\n"
      "\t\"ip\": \"%s\",\n"
      "\t\"rssi\": %i,\n"
      "\t\"httpEnabled\": %u,\n"
      "\t\"mqttEnabled\": %u,\n"
      "\t\"mqttStatus\": %u,\n"
      "\t\"otaEnabled\": %u\n"
    "}",
    getRelayStatus(), getMeasurePowerStatus(), getMeasureFrequency(), getNoiseEnabled(),
    getNoiseValue(), getTimerEnabled(), getTimerElapsedTime(), getTimerTimeout(),
    getWifiStatus(), getWifiMac().c_str(), getWifiSsid().c_str(), getWifiIp().c_str(),
    getWifiRssi(), getHttpEnabled(), getMqttEnabled(), getMqttStatus(), getOtaEnabled());

  return (String)deviceStatusBuffer;
}

void logSerialMessage(String message) {

  if (serialLogEnabled) {
    Serial.println(message);
  }
}

void logMessage(String message) {

  logSerialMessage(message);
  logMqttMessage(message);
}
