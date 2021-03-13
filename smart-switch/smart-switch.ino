#include "config.hpp"
#include "common.hpp"
#include "relay.hpp"
#include "timer.hpp"
#include "noise.hpp"
#include "wifi.hpp"
#include "mqtt.hpp"
#include "http.hpp"

void setup() {

  commonSetup();
  relaySetup();
  noiseSetup();
  wifiSetup();
  httpSetup();
  mqttSetup();

  logSerialMessage("\n--- Setup completed in " + (String)millis() + " ms ---\n\n");
}

void loop() {

  uint32_t currEvalTime = millis();

  evalTimerStatus(currEvalTime);
  evalNoiseStatus(currEvalTime);
  evalWifiStatus(currEvalTime);
  evalHttpStatus(currEvalTime);
  evalMqttStatus(currEvalTime);
}
