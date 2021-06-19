#include "wifi.hpp"

uint32_t lastWifiEvalTime = 0;
bool wifiFailure = false;
bool wifiSettingUp = true;

void wifiSetup() {

	logSerialMessage("\n--- WiFi setup ---");

	WiFi.mode(WIFI_STA);

	char wifiHostname[38];
	getDeviceId(wifiHostname);
	WiFi.hostname(wifiHostname);

	WiFi.begin(wifiSsid, wifiPass);

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

void onWifiConnected() {

	if (wifiSettingUp) {
		wifiSettingUp = false;
		logMessage("Connected to WiFi");
		printWifiInfo();
	} else if (wifiFailure) {
		wifiFailure = false;
		logMessage("WiFi connection re-established");
	}
}

void onWifiFailed() {

	if (!wifiFailure && !wifiSettingUp) {
		wifiFailure = true;
		logSerialMessage("WiFi was disconnected, establishing new connection");
	}
}

bool getWifiStatus() {

	const bool wifiConnected = WiFi.status() == WL_CONNECTED;
	if (wifiConnected) {
		onWifiConnected();
	} else {
		onWifiFailed();
	}
	return wifiConnected;
}

void evalWifiStatus(const uint32_t currEvalTime) {

	if ((uint32_t)(currEvalTime - lastWifiEvalTime) < wifiEvalInterval) {
		return;
	}
	lastWifiEvalTime = currEvalTime;

	if (!getWifiStatus()) {
		logSerialMessage("Connection to WiFi failed");
	};
}
