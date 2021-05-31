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

void getDeviceStatus(char *deviceStatusBuffer) {

	char macBuffer[18];
	getWifiMac(macBuffer);
	char ssidBuffer[33];
	getWifiSsid(ssidBuffer);
	char ipBuffer[16];
	getWifiIp(ipBuffer);

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
		getRelayStatus(), getMeasurePowerStatus(), getMeasureFrequency(), getNoiseEnabled(), getNoiseValue(),
		getTimerEnabled(), getTimerElapsedTime(), getTimerTimeout(), getWifiStatus(), macBuffer, ssidBuffer, ipBuffer,
		getWifiRssi(), getHttpEnabled(), getMqttEnabled(), getMqttStatus(), getOtaEnabled());
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
