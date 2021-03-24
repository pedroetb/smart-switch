#include "mqtt.hpp"

WiFiClient wifiClient;
PubSubClient mqttClient;
String mqttClientId;
String mqttOutputTopic;
String mqttInputTopic;
char messageBuffer[mqttMaxPacketSize + 1];
uint32_t lastMqttEvalTime = 0;
bool previouslyConnected = false;
bool mqttDisconnected = true;
bool mqttEnabled = true;

void mqttSetup() {

  logSerialMessage("\n--- MQTT setup ---");

  mqttClient.setClient(wifiClient);
  mqttClient.setServer(mqttBrokerUrl, mqttBrokerPort);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setBufferSize(mqttMaxPacketSize);
  mqttClient.setKeepAlive(mqttKeepAlive);
  mqttClient.setSocketTimeout(mqttSocketTimeout);

  mqttClientId = (String)rootName + "-" + commonName + "-" + uniqueId;
  mqttOutputTopic = (String)"/" + rootName + "/" + commonName + "/output";
  mqttInputTopic = (String)"/" + rootName + "/" + commonName + "/" + uniqueId + "/input";

  logSerialMessage("MQTT client started, waiting for connection");
}

void logMqttMessage(String message) {

  if (!mqttLogEnabled || !mqttEnabled) {
    return;
  }

  message = "[" + (String)mqttClientId + "] " + message;
  if (mqttClient.connected()) {
    mqttClient.beginPublish(mqttOutputTopic.c_str(), message.length(), false);
    mqttClient.print(message);
    mqttClient.endPublish();
  }
}

void mqttConnect() {

  if (previouslyConnected) {
    logSerialMessage("MQTT was disconnected, establishing new connection");
  }

  if (mqttClient.connect(mqttClientId.c_str(), mqttUser, mqttPass)) {
    logMessage("MQTT connection established");
    previouslyConnected = true;
    mqttDisconnected = false;
    mqttClient.unsubscribe(mqttInputTopic.c_str());
    mqttClient.subscribe(mqttInputTopic.c_str());
  } else {
    logSerialMessage("MQTT connection failed with error: "+  (String)mqttClient.state());
  }
}

void mqttDisconnect() {

  if (mqttDisconnected) {
    return;
  }
  mqttClient.unsubscribe(mqttInputTopic.c_str());
  mqttClient.disconnect();
  mqttDisconnected = true;
  if (previouslyConnected) {
    logSerialMessage("MQTT connection closed");
  }
}

String getMqttRootResponse() {

  String res = "Actions:";
  for (uint8_t i = 0; i < actionsLength; i++) {
    String action = actions[i];
    if (action.equals("/enable-mqtt") || action.equals("/disable-mqtt")) {
      continue;
    }
    if (i > 0) {
      res += ",";
    }
    if (action.equals("/set-timer")) {
      res += " [" + action + " <param>]";
    } else {
      res += " [" + action + "]";
    }
  }

  return res;
}

void handleMqttRequest(String message) {

  uint8_t spacePosition = message.lastIndexOf(" ");
  String command = message.substring(0, spacePosition);
  String parameter = message.substring(spacePosition + 1);

  if (command.equals("/")) {
    logMqttMessage(getMqttRootResponse());
  } else if (command.equals("/on")) {
    enableRelay();
  } else if (command.equals("/off")) {
    disableRelay();
  } else if (command.equals("/toggle")) {
    toggleRelay();
  } else if (command.equals("/status")) {
    logMqttMessage(getDeviceStatus());
  } else if (command.equals("/enable-noise")) {
    enableNoise();
  } else if (command.equals("/disable-noise")) {
    disableNoise();
  } else if (command.equals("/enable-timer")) {
    enableTimer();
  } else if (command.equals("/disable-timer")) {
    disableTimer();
  } else if (command.equals("/set-timer")) {
    setTimerTimeout(parameter);
  } else if (command.equals("/enable-http")) {
    enableHttp();
  } else if (command.equals("/disable-http")) {
    disableHttp();
  } else if (command.equals("/reset")) {
    restartBoard();
  } else {
    logMessage("Invalid MQTT command: " + command);
  }
}

String getStringMessage(uint8_t* payload, uint8_t length) {

  for (uint8_t i = 0; i < length; i++) {
    messageBuffer[i] = (char)payload[i];
  }
  messageBuffer[length] = 0;

  return messageBuffer;
}

void mqttCallback(char* topic, uint8_t* payload, uint8_t length) {

  delay(1000); // TODO needed because of PubSubClient bug (cannot publish from subscribe callback)
  String mqttMessage = getStringMessage(payload, length);
  logMessage("MQTT received: " + mqttMessage);
  handleMqttRequest(mqttMessage);
}

void evalMqttStatus(uint32_t currEvalTime) {

  if ((uint32_t)(currEvalTime - lastMqttEvalTime) < mqttEvalTimeout) {
    return;
  }
  lastMqttEvalTime = currEvalTime;

  if (!getWifiStatus()) {
    mqttDisconnect();
    return;
  }

  if (mqttClient.connected()) {
    mqttClient.loop();
    return;
  }

  if (mqttEnabled) {
    mqttConnect();
  }
}

void enableMqtt() {

  if (mqttEnabled) {
    return;
  }

  mqttEnabled = true;
  mqttConnect();
  logMessage("MQTT communication enabled");
}

void disableMqtt() {

  if (!mqttEnabled) {
    return;
  }

  mqttEnabled = false;
  mqttDisconnect();
  logSerialMessage("MQTT communication disabled");
}

bool getMqttEnabled() {

  return mqttEnabled;
}
