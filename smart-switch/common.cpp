#include "common.hpp"

char deviceId[38];

void setDeviceId() {

	strcpy(deviceId, rootName);
	strcat(deviceId, "-");
	strcat(deviceId, commonName);
	strcat(deviceId, "-");
	strcat(deviceId, uniqueId);
}

void commonSetup() {

	if (serialLogEnabled) {
		Serial.begin(bauds);
	}
	setDeviceId();
	logSerialMessage("\n--- Start smart-switch ---");
}

void restartBoard() {

	delay(500);
	ESP.restart();
}

void getArrayPropertyValue(char *buffer, const char *propName) {

	strcpy(buffer, "[ ");
	for (uint8_t i = 0; i < channelsAvailable; i++) {
		if (i > 0) {
			strcat(buffer, ", ");
		}
		if (strcmp(propName, "powerStatus") == 0) {
			if (getPowerStatus(i)) {
				strcat(buffer, "1");
			} else {
				strcat(buffer, "0");
			}
		} else if (strcmp(propName, "netFrequency") == 0) {
			char tmp[3];
			itoa(getNetFrequency(i), tmp, 10);
			strcat(buffer, tmp);
		} else if (strcmp(propName, "relayStatus") == 0) {
			if (getRelayStatus(i)) {
				strcat(buffer, "1");
			} else {
				strcat(buffer, "0");
			}
		} else if (strcmp(propName, "timerEnabled") == 0) {
			if (getTimerEnabled(i)) {
				strcat(buffer, "1");
			} else {
				strcat(buffer, "0");
			}
		} else if (strcmp(propName, "noiseEnabled") == 0) {
			if (getNoiseEnabled(i)) {
				strcat(buffer, "1");
			} else {
				strcat(buffer, "0");
			}
		}
	}
	strcat(buffer, " ]");
}

void getDeviceId(char *deviceIdBuffer) {

	strcpy(deviceIdBuffer, deviceId);
}

void getDeviceStatus(char *deviceStatusBuffer) {

	static char macBuffer[18];
	if (strlen(macBuffer) == 0) {
		getWifiMac(macBuffer);
	}
	static char ssidBuffer[33];
	if (strlen(ssidBuffer) == 0) {
		getWifiSsid(ssidBuffer);
	}
	static char ipBuffer[16];
	if (strlen(ipBuffer) == 0) {
		getWifiIp(ipBuffer);
	}

	char powerStatusBuffer[arrayValueSize];
	getArrayPropertyValue(powerStatusBuffer, "powerStatus");
	char netFrequencyBuffer[arrayValueSize];
	getArrayPropertyValue(netFrequencyBuffer, "netFrequency");
	char relayStatusBuffer[arrayValueSize];
	getArrayPropertyValue(relayStatusBuffer, "relayStatus");
	char timerEnabledBuffer[arrayValueSize];
	getArrayPropertyValue(timerEnabledBuffer, "timerEnabled");
	char noiseEnabledBuffer[arrayValueSize];
	getArrayPropertyValue(noiseEnabledBuffer, "noiseEnabled");

	snprintf(deviceStatusBuffer, deviceStatusMaxSize, deviceStatusTemplate,
		powerStatusBuffer, netFrequencyBuffer, relayStatusBuffer,
		timerEnabledBuffer, getTimerElapsedTime(), getTimerTimeout(),
		noiseEnabledBuffer, getNoiseValue(),
		getWifiStatus(), macBuffer, ssidBuffer, ipBuffer, getWifiRssi(),
		getHttpEnabled(), getMqttEnabled(), getMqttStatus(), getOtaEnabled());
}

bool validateChannel(const uint8_t channel) {

	const bool validChannel = channel > 0 && channel <= channelsAvailable;
	if (!validChannel) {
		logMessage("Received invalid channel to operate");
	}
	return validChannel;
}

bool isActionByChannel(const char *action) {

	return strcmp(action, "/on") == 0 || strcmp(action, "/off") == 0 || strcmp(action, "/toggle") == 0 ||
		strcmp(action, "/enable-noise") == 0 || strcmp(action, "/disable-noise") == 0 ||
		strcmp(action, "/enable-timer") == 0 || strcmp(action, "/disable-timer") == 0;
}

void logSerialMessage(const char *message) {

	if (serialLogEnabled) {
		Serial.println(message);
	}
}

void logMessage(const char *message) {

	logSerialMessage(message);
	logMqttMessage(message);
}
