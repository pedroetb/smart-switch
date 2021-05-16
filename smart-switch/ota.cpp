#include "ota.hpp"

String otaHostname;
uint32_t lastOtaEvalTime = 0;
int8_t lastPercentage = -1;
bool otaEnabled = false;
bool otaListening = false;

void onOtaStart() {

	mqttConnect();
	String updateType;
	if (ArduinoOTA.getCommand() == U_FLASH) {
		updateType = "sketch";
	} else {
		updateType = "filesystem";
	}
	logMessage("OTA update (" + updateType + ") started");
}

void onOtaProgress(uint32_t progress, uint32_t total) {

	int8_t percentage = progress * 100 / total;
	uint8_t progressMarkReached = !(percentage % 25);
	if (progressMarkReached && percentage > lastPercentage) {
		logMessage("OTA update progress: " + (String)percentage + "%");
		lastPercentage = percentage;
	}
}

void onOtaEnd() {

	logMessage("OTA update ended, restarting..");
	restartBoard();
}

void onOtaError(ota_error_t error) {

	String errorType;
	if (error == OTA_AUTH_ERROR) {
		errorType = "Auth";
	} else if (error == OTA_BEGIN_ERROR) {
		errorType = "Begin";
	} else if (error == OTA_CONNECT_ERROR) {
		errorType = "Connect";
	} else if (error == OTA_RECEIVE_ERROR) {
		errorType = "Receive";
	} else if (error == OTA_END_ERROR) {
		errorType = "End";
	} else {
		errorType = "Unknown";
	}
	logMessage("OTA update failed: " + (String)errorType + " (" + (String)error + ") error");
}

void otaSetup() {

	logSerialMessage("\n--- OTA setup ---");

	otaHostname = (String)rootName + "-" + commonName + "-" + uniqueId;

	ArduinoOTA.setHostname(otaHostname.c_str());
	ArduinoOTA.setPort(otaPort);
	ArduinoOTA.setRebootOnSuccess(false);

	ArduinoOTA.onStart(onOtaStart);
	ArduinoOTA.onProgress(onOtaProgress);
	ArduinoOTA.onEnd(onOtaEnd);
	ArduinoOTA.onError(onOtaError);

	logSerialMessage("OTA configured at " + otaHostname + ":" + (String)otaPort + ", disabled by default");
}

void otaConnect() {

	if (otaListening) {
		return;
	}

	otaListening = true;
	ArduinoOTA.begin();
	logMessage("OTA listening on " + ArduinoOTA.getHostname() + ":" + (String)otaPort);
}

void evalOtaStatus(uint32_t currEvalTime) {

	if (!otaEnabled || !getWifiStatus()) {
		return;
	}

	if ((uint32_t)(currEvalTime - lastOtaEvalTime) < otaEvalTimeout) {
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
