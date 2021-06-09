#include "common.hpp"

constexpr uint8_t arrayValueLength = 4 * channelsAvailable + 3;
constexpr char deviceStatusTemplate[430] = "{\n"
	"\t\"powerStatus\": %s,\n"
	"\t\"netFrequency\": %s,\n"
	"\t\"relayStatus\": %s,\n"
	"\t\"timerEnabled\": %u,\n"
	"\t\"timerElapsedTime\": %u,\n"
	"\t\"timerTimeout\": %u,\n"
	"\t\"noiseEnabled\": %u,\n"
	"\t\"noiseValue\": %u,\n"
	"\t\"wifiStatus\": %u,\n"
	"\t\"mac\": \"%s\",\n"
	"\t\"ssid\": \"%s\",\n"
	"\t\"ip\": \"%s\",\n"
	"\t\"rssi\": %i,\n"
	"\t\"httpEnabled\": %u,\n"
	"\t\"mqttEnabled\": %u,\n"
	"\t\"mqttStatus\": %u,\n"
	"\t\"otaEnabled\": %u\n"
	"}";

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
		}
	}
	strcat(buffer, " ]");
}

void getDeviceStatus(char *deviceStatusBuffer) {

	char macBuffer[18];
	getWifiMac(macBuffer);
	char ssidBuffer[33];
	getWifiSsid(ssidBuffer);
	char ipBuffer[16];
	getWifiIp(ipBuffer);

	char powerStatusBuffer[arrayValueLength];
	getArrayPropertyValue(powerStatusBuffer, "powerStatus");
	char netFrequencyBuffer[arrayValueLength];
	getArrayPropertyValue(netFrequencyBuffer, "netFrequency");
	char relayStatusBuffer[arrayValueLength];
	getArrayPropertyValue(relayStatusBuffer, "relayStatus");

	snprintf(deviceStatusBuffer, 420, deviceStatusTemplate,
		powerStatusBuffer, netFrequencyBuffer, relayStatusBuffer,
		getTimerEnabled(), getTimerElapsedTime(), getTimerTimeout(), getNoiseEnabled(), getNoiseValue(),
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

void logSerialMessage(const char *message) {

	if (serialLogEnabled) {
		Serial.println(message);
	}
}

void logMessage(const char *message) {

	logSerialMessage(message);
	logMqttMessage(message);
}
