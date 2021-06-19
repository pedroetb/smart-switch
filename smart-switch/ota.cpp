#include "ota.hpp"

uint32_t lastOtaEvalTime = 0;
int8_t lastPercentage = -1;
bool otaEnabled = false;
bool otaListening = false;

void logOtaStart(const char *updateType) {

	char msg[32] = "OTA update (";
	strcat(msg, updateType);
	strcat(msg, ") started");
	logMessage(msg);
}

void onOtaStart() {

	switchOff();
	disableTimer();
	disableNoise();
	disableHttp();

	mqttConnect();

	char updateType[11];
	if (ArduinoOTA.getCommand() == U_FLASH) {
		strcpy(updateType, "sketch");
	} else {
		strcpy(updateType, "filesystem");
	}

	logOtaStart(updateType);
}

void logOtaProgress(const uint8_t percentage) {

	char msg[26] = "OTA update progress: ";
	char tmp[4];
	itoa(percentage, tmp, 10);
	strcat(msg, tmp);
	strcat(msg, "%");
	logMessage(msg);
}

void onOtaProgress(const uint32_t progress, const uint32_t total) {

	const int8_t percentage = progress * 100 / total;
	const uint8_t progressMarkReached = !(percentage % 25);
	if (progressMarkReached && percentage > lastPercentage) {
		logOtaProgress(percentage);
		lastPercentage = percentage;
	}
}

void onOtaEnd() {

	logMessage("OTA update ended, restarting..");
	restartBoard();
}

void logOtaError(const char *errorType) {

	char msg[33] = "OTA update failed: ";
	strcat(msg, errorType);
	strcat(msg, " error");
	logMessage(msg);
}

void onOtaError(const ota_error_t error) {

	char errorType[8] = "Unknown";
	if (error == OTA_AUTH_ERROR) {
		strcpy(errorType, "Auth");
	} else if (error == OTA_BEGIN_ERROR) {
		strcpy(errorType, "Begin");
	} else if (error == OTA_CONNECT_ERROR) {
		strcpy(errorType, "Connect");
	} else if (error == OTA_RECEIVE_ERROR) {
		strcpy(errorType, "Receive");
	} else if (error == OTA_END_ERROR) {
		strcpy(errorType, "End");
	}
	logOtaError(errorType);
	restartBoard();
}

void logOtaSetupEnd(const char *otaHostname) {

	char msg[110] = "OTA configured at ";
	strcat(msg, otaHostname);
	strcat(msg, ":");
	char port[6];
	itoa(otaPort, port, 10);
	strcat(msg, port);
	strcat(msg, ", disabled by default");
	logSerialMessage(msg);
}

void otaSetup() {

	logSerialMessage("\n--- OTA setup ---");

	char otaHostname[38];
	getDeviceId(otaHostname);

	ArduinoOTA.setHostname(otaHostname);
	ArduinoOTA.setPort(otaPort);
	ArduinoOTA.setRebootOnSuccess(false);

	ArduinoOTA.onStart(onOtaStart);
	ArduinoOTA.onProgress(onOtaProgress);
	ArduinoOTA.onEnd(onOtaEnd);
	ArduinoOTA.onError(onOtaError);

	logOtaSetupEnd(otaHostname);
}

void logOtaConnect() {

	char msg[39] = "OTA listening on ";
	char ipBuffer[16];
	getWifiIp(ipBuffer);
	strcat(msg, ipBuffer);
	strcat(msg, ":");
	char port[6];
	itoa(otaPort, port, 10);
	strcat(msg, port);
	logMessage(msg);
}

void otaConnect() {

	if (otaListening) {
		return;
	}

	otaListening = true;
	ArduinoOTA.begin();
	logOtaConnect();
}

void evalOtaStatus(const uint32_t currEvalTime) {

	if (!otaEnabled || !getWifiStatus()) {
		return;
	}

	if ((uint32_t)(currEvalTime - lastOtaEvalTime) < otaEvalInterval) {
		return;
	}
	lastOtaEvalTime = currEvalTime;

	otaConnect();
	ArduinoOTA.handle();
}

void enableOta() {

	if (otaEnabled) {
		return;
	}

	otaEnabled = true;
	logMessage("OTA enabled");
}

void disableOta() {

	if (!otaEnabled) {
		return;
	}

	otaEnabled = false;
	logMessage("OTA disabled");
}

bool getOtaEnabled() {

	return otaEnabled;
}
