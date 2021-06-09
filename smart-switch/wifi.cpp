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

void getWifiMac(char *macBuffer) {

	uint8_t mac[6];
	WiFi.macAddress(mac);

	snprintf(macBuffer, 18, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}

void getWifiSsid(char *ssidBuffer) {

	strcpy(ssidBuffer, WiFi.SSID().c_str());
}

void getWifiIp(char *ipBuffer) {

	IPAddress ip = WiFi.localIP();
	char tmp[4];
	itoa(ip[0], tmp, 10);
	strcpy(ipBuffer, tmp);
	strcat(ipBuffer, ".");
	itoa(ip[1], tmp, 10);
	strcat(ipBuffer, tmp);
	strcat(ipBuffer, ".");
	itoa(ip[2], tmp, 10);
	strcat(ipBuffer, tmp);
	strcat(ipBuffer, ".");
	itoa(ip[3], tmp, 10);
	strcat(ipBuffer, tmp);
}

int32_t getWifiRssi() {

	return WiFi.RSSI();
}

bool getWifiStatus() {

	return WiFi.status() == WL_CONNECTED;
}

void printWifiInfo() {

	logSerialMessage("\n--- WiFi status ---");

	char msg[64];
	char tmp[33];

	strcpy(msg, "MAC address: ");
	getWifiMac(tmp);
	strcat(msg, tmp);
	logSerialMessage(msg);

	strcpy(msg, "SSID: ");
	getWifiSsid(tmp);
	strcat(msg, tmp);
	logSerialMessage(msg);

	strcpy(msg, "IP address: ");
	getWifiIp(tmp);
	strcat(msg, tmp);
	logSerialMessage(msg);

	strcpy(msg, "Signal strength (RSSI) in dBm: ");
	ltoa(getWifiRssi(), tmp, 10);
	strcat(msg, tmp);
	strcat(msg, "\n");
	logSerialMessage(msg);
}

void notifyWifiReconnected() {

	if (wifiFailure) {
		wifiFailure = false;
		logMessage("WiFi connection restablished");
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
			logMessage("Connected to WiFi");
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
