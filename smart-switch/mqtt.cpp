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

void getMqttErrorMeaning(char *message, const int8_t state) {

	strcat(message, " (");
	if (state == -4) {
		strcat(message, "connection timeout, keepalive exceeded");
	} else if (state == -3) {
		strcat(message, "connection lost");
	} else if (state == -2) {
		strcat(message, "connection failed");
	} else if (state == -1) {
		strcat(message, "connection closed normally");
	} else if (state == 1) {
		strcat(message, "bad protocol, MQTT version not supported");
	} else if (state == 2) {
		strcat(message, "client ID rejected");
	} else if (state == 3) {
		strcat(message, "server unavailable");
	} else if (state == 4) {
		strcat(message, "user credentials rejected");
	} else if (state == 5) {
		strcat(message, "client connection unauthorized");
	} else {
		strcat(message, "unknown");
	}
	strcat(message, ")");
}

void logMqttConnectionError() {

	int8_t state = mqttClient.state();
	char msg[81] = "MQTT connection failed with error: ";
	char tmp[3];
	itoa(state, tmp, 10);
	strcat(msg, tmp);
	getMqttErrorMeaning(msg, state);
	logSerialMessage(msg);
}

void mqttConnect() {

	if (getMqttStatus()) {
		return;
	}

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
		strncpy(cmd, mqttMsg, 28);
		cmd[28] = '\0';
		param[0] = '\0';
	} else {
		uint8_t separatorPosition = separatorLocation - mqttMsg;

		strncpy(cmd, mqttMsg, separatorPosition);
		cmd[separatorPosition] = '\0';

		strncpy(param, mqttMsg + separatorPosition + 1, 27);
		param[27] = '\0';
	}
}

void getMqttRootResponse(char *res) {

	strcpy(res, "Actions:");
	char action[maxActionSize];
	for (uint8_t i = 0; i < actionsLength; i++) {
		strcpy(action, actions[i]);
		if (strcmp(action, "/enable-mqtt") == 0 || strcmp(action, "/disable-mqtt") == 0) {
			continue;
		}
		strcat(res, "\n\t");
		strcat(res, action);
		if (strcmp(action, "/on") == 0 || strcmp(action, "/off") == 0 || strcmp(action, "/toggle") == 0 ||
			strcmp(action, "/enable-noise") == 0 || strcmp(action, "/disable-noise") == 0) {

			strcat(res, " [<channel>]");
		} else if (strcmp(action, "/set-timer") == 0) {
			strcat(res, " <timeout>");
		}
	}
}

void onMqttRootRequest() {

	static char mqttResponse[300];
	if (strlen(mqttResponse) == 0) {
		getMqttRootResponse(mqttResponse);
	}
	logMqttMessage(mqttResponse);
}

void logInvalidMqttParamReceived(const char *cmd) {

	char msg[60] = "Received invalid parameter for '";
	strcat(msg, cmd);
	strcat(msg, "' MQTT action");
	logMessage(msg);
}

void onMqttActionByChannelRequest(const char *cmd, const char *param) {

	uint8_t paramLength = strlen(param);
	if (paramLength == 0) {
		if (strcmp(cmd, "/on") == 0) {
			switchOn();
		} else if (strcmp(cmd, "/off") == 0) {
			switchOff();
		} else if (strcmp(cmd, "/toggle") == 0) {
			toggleRelay();
		} else if (strcmp(cmd, "/enable-noise") == 0) {
			enableNoise();
		} else if (strcmp(cmd, "/disable-noise") == 0) {
			disableNoise();
		}
	} else if (paramLength < 4) {
		uint8_t channel = atoi(param);
		if (!validateChannel(channel)) {
			return;
		}
		channel--;
		if (strcmp(cmd, "/on") == 0) {
			switchOn(channel);
		} else if (strcmp(cmd, "/off") == 0) {
			switchOff(channel);
		} else if (strcmp(cmd, "/toggle") == 0) {
			toggleRelay(channel);
		} else if (strcmp(cmd, "/enable-noise") == 0) {
			enableNoise(channel);
		} else if (strcmp(cmd, "/disable-noise") == 0) {
			disableNoise(channel);
		}
	} else {
		logInvalidMqttParamReceived(cmd);
	}
}

void onMqttSetTimerRequest(const char *cmd, const char *param) {

	uint8_t paramLength = strlen(param);
	if (paramLength > 0 && paramLength < 11) {
		setTimerTimeout(param);
	} else {
		logInvalidMqttParamReceived(cmd);
	}
}

void onMqttStatusRequest() {

	char status[deviceStatusMaxSize];
	getDeviceStatus(status);
	char msg[deviceStatusMaxSize + 9] = "Status:\n";
	strcat(msg, status);
	logMqttMessage(msg);
}

void onMqttInvalidRequest(const char *cmd) {

	char msg[51] = "Invalid MQTT command: ";
	strcat(msg, cmd);

	logMessage(msg);
}

void handleMqttRequest(const char *mqttMsg) {

	char cmd[29];
	char param[28];
	getMqttCommandAndParam(cmd, param, mqttMsg);

	if (strcmp(cmd, "/") == 0) {
		onMqttRootRequest();
	} else if (strcmp(cmd, "/on") == 0 || strcmp(cmd, "/off") == 0 || strcmp(cmd, "/toggle") == 0 ||
		strcmp(cmd, "/enable-noise") == 0 || strcmp(cmd, "/disable-noise") == 0) {

		onMqttActionByChannelRequest(cmd, param);
	} else if (strcmp(cmd, "/status") == 0) {
		onMqttStatusRequest();
	} else if (strcmp(cmd, "/enable-timer") == 0) {
		enableTimer();
	} else if (strcmp(cmd, "/disable-timer") == 0) {
		disableTimer();
	} else if (strcmp(cmd, "/set-timer") == 0) {
		onMqttSetTimerRequest(cmd, param);
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

	char msg[44] = "MQTT received: ";
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

	mqttConnect();
	mqttClient.loop();
}

void enableMqtt() {

	if (mqttEnabled) {
		return;
	}

	mqttEnabled = true;
	logMessage("MQTT communication enabled");
	mqttConnect();
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
