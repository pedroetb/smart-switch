#include "wifi.hpp"

uint32_t lastWifiConnectTime = 0;
uint32_t lastWifiEvalTime = 0;
bool wifiFailure = false;
bool wifiSettingUp = true;
uint8_t connectRetriesSpent = 0;

void wifiSetup() {

	logSerialMessage("\n--- WiFi setup ---");

	WiFi.mode(WIFI_STA);

	logSerialMessage("WiFi connection is starting");
}

String getWifiMac() {

	uint8_t mac[6];
	WiFi.macAddress(mac);

	char macBuffer[17];
	sprintf(macBuffer, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

	return macBuffer;
}

String getWifiSsid() {

	return WiFi.SSID();
}

String getWifiIp() {

	return WiFi.localIP().toString();
}

int32_t getWifiRssi() {

	return WiFi.RSSI();
}

bool getWifiStatus() {

	return WiFi.status() == WL_CONNECTED;
}

void printWifiInfo() {

	logSerialMessage("\n--- WiFi status ---");
	logSerialMessage("MAC address: " + getWifiMac());
	logSerialMessage("SSID: " + getWifiSsid());
	logSerialMessage("IP address: " + getWifiIp());
	logSerialMessage("Signal strength (RSSI) in dBm: " + String(getWifiRssi()) + "\n");
}

void notifyWifiReconnected() {

	if (wifiFailure) {
		wifiFailure = false;
		logSerialMessage("WiFi connection restablished");
	}
}

void wifiConnect(uint32_t currWifiConnectTime) {

	if ((uint32_t)(currWifiConnectTime - lastWifiConnectTime) < wifiConnectTimeout && !wifiSettingUp) {
		return;
	}

	lastWifiConnectTime = currWifiConnectTime;

	if (wifiSettingUp) {
		wifiSettingUp = false;
	}

	if (connectRetriesSpent == 0) {
		WiFi.begin(wifiSsid, wifiPass);
		logSerialMessage("Connecting to WiFi");
	}

	if (connectRetriesSpent < wifiConnectRetries) {
		if (getWifiStatus()) {
			connectRetriesSpent = 0;
			lastWifiConnectTime = 0;
			logSerialMessage("Connected to WiFi");
			printWifiInfo();
			notifyWifiReconnected();
			return;
		} else {
			connectRetriesSpent++;
		}
	} else {
		connectRetriesSpent = 0;
		logSerialMessage("Connection to WiFi failed");
	}
}

void evalWifiStatus(uint32_t currEvalTime) {

	if (connectRetriesSpent > 0 || wifiSettingUp) {
		wifiConnect(currEvalTime);
		return;
	}

	if ((uint32_t)(currEvalTime - lastWifiEvalTime) < wifiEvalTimeout) {
		return;
	}

	lastWifiEvalTime = currEvalTime;

	if (getWifiStatus()) {
		if (wifiFailure) {
			notifyWifiReconnected();
			printWifiInfo();
		}
		return;
	}

	wifiFailure = true;
	logSerialMessage("WiFi was disconnected, establishing new connection");
	wifiConnect(currEvalTime);
}
