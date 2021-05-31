#include "mqtt.hpp"

WiFiClient wifiClient;
PubSubClient mqttClient;
char mqttClientId[38];
char mqttOutputTopic[41];
char mqttInputTopic[45];
uint32_t lastMqttEvalTime = 0;
bool previouslyConnected = false;
bool mqttDisconnected = true;
bool mqttEnabled = true;

void setMqttClientId() {

	strcpy(mqttClientId, rootName);
	strcat(mqttClientId, "-");
	strcat(mqttClientId, commonName);
	strcat(mqttClientId, "-");
	strcat(mqttClientId, uniqueId);
}

void setMqttOutputTopic() {

	strcpy(mqttOutputTopic, "/");
	strcat(mqttOutputTopic, rootName);
	strcat(mqttOutputTopic, "/");
	strcat(mqttOutputTopic, commonName);
	strcat(mqttOutputTopic, "/output");
}

void setMqttInputTopic() {

	strcpy(mqttInputTopic, "/");
	strcat(mqttInputTopic, rootName);
	strcat(mqttInputTopic, "/");
	strcat(mqttInputTopic, commonName);
	strcat(mqttInputTopic, "/");
	strcat(mqttInputTopic, uniqueId);
	strcat(mqttInputTopic, "/input");
}

void mqttSetup() {

	logSerialMessage("\n--- MQTT setup ---");

	mqttClient.setClient(wifiClient);
	mqttClient.setServer(mqttBrokerUrl, mqttBrokerPort);
	mqttClient.setCallback(mqttCallback);
	mqttClient.setBufferSize(mqttMaxPacketSize);
	mqttClient.setKeepAlive(mqttKeepAlive);
	mqttClient.setSocketTimeout(mqttSocketTimeout);

	setMqttClientId();
	setMqttOutputTopic();
	setMqttInputTopic();

	logSerialMessage("MQTT client started, waiting for connection");
}

bool getMqttStatus() {

	return mqttClient.connected();
}

void logMqttMessage(const char *message) {

	if (!mqttLogEnabled || !mqttEnabled) {
		return;
	}

	char msg[512] = "[";
	strcat(msg, mqttClientId);
	strcat(msg, "] ");
	strcat(msg, message);

	if (getMqttStatus()) {
		mqttClient.beginPublish(mqttOutputTopic, strlen(msg), false);
		mqttClient.print(msg);
		mqttClient.endPublish();
	}
}

void logMqttConnectionError() {

	char msg[38] = "MQTT connection failed with error: ";
	char tmp[3];
	itoa(mqttClient.state(), tmp, 10);
	strcat(msg, tmp);
	logSerialMessage(msg);
}

void mqttConnect() {

	if (previouslyConnected) {
		logSerialMessage("MQTT was disconnected, establishing new connection");
	}

	if (mqttClient.connect(mqttClientId, mqttUser, mqttPass)) {
		logMessage("MQTT connection established");
		previouslyConnected = true;
		mqttDisconnected = false;
		mqttClient.unsubscribe(mqttInputTopic);
		mqttClient.subscribe(mqttInputTopic);
	} else {
		logMqttConnectionError();
	}
}

void mqttDisconnect() {

	if (mqttDisconnected) {
		return;
	}
	mqttClient.unsubscribe(mqttInputTopic);
	mqttClient.disconnect();
	mqttDisconnected = true;
	if (previouslyConnected) {
		logSerialMessage("MQTT connection closed");
	}
}

void getMqttCommandAndParam(char *cmd, char *param, const char *mqttMsg) {

	char *separatorLocation = strchr(mqttMsg, ' ');

	if (separatorLocation == '\0') {
		strcpy(cmd, mqttMsg);
		param[0] = '\0';
	} else {
		uint8_t separatorPosition = separatorLocation - mqttMsg;

		strncpy(cmd, mqttMsg, separatorPosition);
		cmd[separatorPosition] = '\0';

		strcpy(param, mqttMsg + separatorPosition + 1);
	}
}

void getMqttRootResponse(char *res) {

	char action[16] = "Actions:";
	for (uint8_t i = 0; i < actionsLength; i++) {
		strcpy(action, actions[i]);
		if (strcmp(action, "/enable-mqtt") == 0 || strcmp(action, "/disable-mqtt") == 0) {
			continue;
		}
		if (i > 0) {
			strcat(res, ",");
		}
		strcat(res, "[");
		strcat(res, action);
		if (strcmp(action, "/set-timer") == 0) {
			strcat(res, " <param>");
		}
		strcat(res, "]");
	}
}

void onMqttRootRequest() {

	char msg[500];
	getMqttRootResponse(msg);

	logMqttMessage(msg);
}

void onMqttStatusRequest() {

	char status[400];
	getDeviceStatus(status);

	logMqttMessage(status);
}

void onMqttInvalidRequest(const char *cmd) {

	char msg[150] = "Invalid MQTT command: ";
	strcat(msg, cmd);

	logMqttMessage(msg);
}

void handleMqttRequest(const char *mqttMsg) {

	char cmd[17];
	char param[11];
	getMqttCommandAndParam(cmd, param, mqttMsg);

	if (strcmp(cmd, "/") == 0) {
		onMqttRootRequest();
	} else if (strcmp(cmd, "/on") == 0) {
		switchOn();
	} else if (strcmp(cmd, "/off") == 0) {
		switchOff();
	} else if (strcmp(cmd, "/toggle") == 0) {
		toggleRelay();
	} else if (strcmp(cmd, "/status") == 0) {
		onMqttStatusRequest();
	} else if (strcmp(cmd, "/enable-noise") == 0) {
		enableNoise();
	} else if (strcmp(cmd, "/disable-noise") == 0) {
		disableNoise();
	} else if (strcmp(cmd, "/enable-timer") == 0) {
		enableTimer();
	} else if (strcmp(cmd, "/disable-timer") == 0) {
		disableTimer();
	} else if (strcmp(cmd, "/set-timer") == 0) {
		setTimerTimeout(param);
	} else if (strcmp(cmd, "/enable-http") == 0) {
		enableHttp();
	} else if (strcmp(cmd, "/disable-http") == 0) {
		disableHttp();
	} else if (strcmp(cmd, "/enable-ota") == 0) {
		enableOta();
	} else if (strcmp(cmd, "/disable-ota") == 0) {
		disableOta();
	} else if (strcmp(cmd, "/reset") == 0) {
		restartBoard();
	} else {
		onMqttInvalidRequest(cmd);
	}
}

void getMqttMessage(char *mqttMsg, uint8_t *payload, uint8_t length) {

	for (uint8_t i = 0; i < length; i++) {
		mqttMsg[i] = (char)payload[i];
	}
	mqttMsg[length] = '\0';
}

void logMqttReceivedMessage(const char *mqttMsg) {

	char msg[143] = "MQTT received: ";
	strcat(msg, mqttMsg);

	logMessage(msg);
}

void mqttCallback(char *topic, uint8_t *payload, uint8_t length) {

	delay(1000); // TODO needed because of PubSubClient bug (cannot publish from subscribe callback)
	char messageBuffer[length + 1];
	getMqttMessage(messageBuffer, payload, length);
	logMqttReceivedMessage(messageBuffer);
	handleMqttRequest(messageBuffer);
}

void evalMqttStatus(uint32_t currEvalTime) {

	if (!mqttEnabled || !getWifiStatus()) {
		mqttDisconnect();
		return;
	}

	if ((uint32_t)(currEvalTime - lastMqttEvalTime) < mqttEvalTimeout) {
		return;
	}
	lastMqttEvalTime = currEvalTime;

	if (!getMqttStatus()) {
		mqttConnect();
	}
	mqttClient.loop();
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
	logMessage("MQTT communication disabled");
	mqttDisconnect();
}

bool getMqttEnabled() {

	return mqttEnabled;
}
