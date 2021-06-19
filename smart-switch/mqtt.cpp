#include "mqtt.hpp"

constexpr uint8_t mqttMaxPayloadSize = maxActionSize + maxParamSize; // max. length 25

WiFiClient wifiClient;
PubSubClient mqttClient;
char mqttClientId[38];
char mqttLogTopic[42];
char mqttOutputTopic[45];
char mqttInputTopic[44];
uint32_t lastMqttEvalTime = 0;
uint32_t lastMqttReconnectTime = 0;
bool previouslyConnected = false;
bool mqttConectionLost = true;
bool mqttEnabled = true;

void setMqttClientId() {

	strcpy(mqttClientId, rootName);
	strcat(mqttClientId, "-");
	strcat(mqttClientId, commonName);
	strcat(mqttClientId, "-");
	strcat(mqttClientId, uniqueId);
}

void setMqttTopics() {

	char mqttTopicBase[38];
	strcpy(mqttTopicBase, rootName);
	strcat(mqttTopicBase, "/");
	strcat(mqttTopicBase, commonName);
	strcat(mqttTopicBase, "/");
	strcat(mqttTopicBase, uniqueId);

	strcpy(mqttLogTopic, mqttTopicBase);
	strcat(mqttLogTopic, "/log");

	strcpy(mqttOutputTopic, mqttTopicBase);
	strcat(mqttOutputTopic, "/output");

	strcpy(mqttInputTopic, mqttTopicBase);
	strcat(mqttInputTopic, "/input");
}

void mqttSetup() {

	logSerialMessage("\n--- MQTT setup ---");

	wifiClient.setTimeout(mqttResolveTimeout);

	mqttClient.setClient(wifiClient);
	mqttClient.setServer(mqttBrokerUrl, mqttBrokerPort);
	mqttClient.setCallback(mqttCallback);
	mqttClient.setBufferSize(mqttMaxPacketSize);
	mqttClient.setKeepAlive(mqttKeepAlive);
	mqttClient.setSocketTimeout(mqttSocketTimeout);

	setMqttClientId();
	setMqttTopics();

	logSerialMessage("MQTT client started, waiting for connection");
}

bool getMqttStatus() {

	return mqttClient.connected();
}

void publishMqttMessage(const char *message, const char *topic, bool retain = false) {

	const uint16_t length = strlen(message);

	if (getMqttStatus()) {
		mqttClient.beginPublish(topic, length, retain);
		for (uint16_t i = 0; i < length; i++) {
			mqttClient.write((uint8_t)message[i]);
		}
		mqttClient.endPublish();
	}
}

void logMqttMessage(const char *message) {

	if (!mqttLogEnabled || !mqttEnabled) {
		return;
	}

	publishMqttMessage(message, mqttLogTopic);
}

void outputMqttMessage(const char *message, bool retain = false) {

	if (!mqttEnabled) {
		return;
	}

	publishMqttMessage(message, mqttOutputTopic, retain);
}

void onMqttConnected() {

	logMessage("MQTT connection established");
	previouslyConnected = true;
	mqttConectionLost = false;
	mqttClient.unsubscribe(mqttInputTopic);
	mqttClient.subscribe(mqttInputTopic);
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

	const int8_t state = mqttClient.state();
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
		previouslyConnected = false;
	}

	if (mqttClient.connect(mqttClientId, mqttUser, mqttPass)) {
		onMqttConnected();
	} else {
		logMqttConnectionError();
	}
}

void mqttDisconnect() {

	if (!getMqttStatus()) {
		return;
	}
	mqttClient.unsubscribe(mqttInputTopic);
	mqttClient.disconnect();
	if (previouslyConnected) {
		logSerialMessage("MQTT connection closed");
	}
}

void getMqttCommandAndParam(char *cmd, char *param, const char *mqttMsg) {

	const char *separatorLocation = strchr(mqttMsg, ' ');

	if (separatorLocation == '\0') {
		strncpy(cmd, mqttMsg, mqttMaxPayloadSize);
		cmd[mqttMaxPayloadSize - 1] = '\0';
		param[0] = '\0';
	} else {
		uint8_t separatorPosition = separatorLocation - mqttMsg;

		strncpy(cmd, mqttMsg, separatorPosition);
		cmd[separatorPosition] = '\0';

		strncpy(param, mqttMsg + separatorPosition + 1, mqttMaxPayloadSize - 1);
		param[mqttMaxPayloadSize - 2] = '\0';
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
		if (isActionByChannel(action)) {
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
	outputMqttMessage(mqttResponse);
}

void logInvalidMqttParamReceived(const char *cmd) {

	char msg[maxActionSize + 45] = "Received invalid parameter for '";
	strcat(msg, cmd);
	strcat(msg, "' MQTT action");
	logMessage(msg);
}

void onMqttActionByChannelRequest(const char *cmd, const char *param) {

	const uint8_t paramLength = strlen(param);
	if (paramLength > 3) {
		logInvalidMqttParamReceived(cmd);
		return;
	}

	uint8_t channel;
	bool channelProvided = false;
	if (paramLength != 0) {
		channelProvided = true;
		channel = atoi(param);
		if (validateChannel(channel)) {
			channel--;
		}
	}

	if (strcmp(cmd, "/on") == 0) {
		if (channelProvided) {
			switchOn(channel);
		} else {
			switchOn();
		}
	} else if (strcmp(cmd, "/off") == 0) {
		if (channelProvided) {
			switchOff(channel);
		} else {
			switchOff();
		}
	} else if (strcmp(cmd, "/toggle") == 0) {
		if (channelProvided) {
			toggleRelay(channel);
		} else {
			toggleRelay();
		}
	} else if (strcmp(cmd, "/enable-noise") == 0) {
		if (channelProvided) {
			enableNoise(channel);
		} else {
			enableNoise();
		}
	} else if (strcmp(cmd, "/disable-noise") == 0) {
		if (channelProvided) {
			disableNoise(channel);
		} else {
			disableNoise();
		}
	}
}

void onMqttSetTimerRequest(const char *cmd, const char *param) {

	const uint8_t paramLength = strlen(param);
	if (paramLength > 0 && paramLength < maxParamSize) {
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
	outputMqttMessage(msg, true);
}

void onMqttInvalidRequest(const char *cmd) {

	char msg[mqttMaxPayloadSize + 22] = "Invalid MQTT command: ";
	strcat(msg, cmd);
	logMessage(msg);
}

void handleMqttRequest(const char *mqttMsg) {

	char cmd[mqttMaxPayloadSize];
	char param[mqttMaxPayloadSize - 1];
	getMqttCommandAndParam(cmd, param, mqttMsg);

	if (strcmp(cmd, "/") == 0) {
		onMqttRootRequest();
	} else if (isActionByChannel(cmd)) {
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

	char msg[mqttMaxPayloadSize + 15] = "MQTT received: ";
	strcat(msg, mqttMsg);

	logMessage(msg);
}

void mqttCallback(char *topic, uint8_t *payload, uint8_t length) {

	if (length >= mqttMaxPayloadSize) {
		logMessage("Received too long MQTT payload");
		return;
	}

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

	if ((uint32_t)(currEvalTime - lastMqttReconnectTime) >= mqttReconnectInterval) {
		lastMqttReconnectTime = currEvalTime;
		mqttConnect();
	}

	if ((uint32_t)(currEvalTime - lastMqttEvalTime) < mqttEvalInterval) {
		return;
	}
	lastMqttEvalTime = currEvalTime;

	if (!mqttClient.loop()) {
		if (previouslyConnected && !mqttConectionLost) {
			mqttConectionLost = true;
			logSerialMessage("MQTT connection lost");
		}
		return;
	}
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
