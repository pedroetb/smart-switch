#include "http.hpp"

ESP8266WebServer server(httpServerPort);
char httpTitle[38];
uint32_t lastHttpEvalTime = 0;
bool httpEnabled = true;

void logHttpRequestUri() {

	char msg[httpMaxItemLength + 20] = "HTTP received uri: ";
	strncat(msg, server.uri().c_str(), httpMaxItemLength);
	logMessage(msg);
}

void logHttpRequestArg(const uint8_t &i) {

	char msg[2 * httpMaxItemLength + 21] = "HTTP received arg: ";
	strncat(msg, server.argName(i).c_str(), httpMaxItemLength);
	strcat(msg, "=");
	strncat(msg, server.arg(i).c_str(), httpMaxItemLength);
	logMessage(msg);
}

void printHttpRequest() {

	logHttpRequestUri();
	uint8_t argsCount = server.args();
	for (uint8_t i = 0; i < argsCount; i++) {
		logHttpRequestArg(i);
	}
}

void getHttpParamValue(char *paramValue, const char *paramName) {

	strncpy(paramValue, server.arg(paramName).c_str(), httpMaxItemLength);
	paramValue[httpMaxItemLength - 1] = '\0';
}

bool checkFormatIsHtml() {

	if (server.hasArg("format")) {
		char format[httpMaxItemLength];
		getHttpParamValue(format, "format");
		return strcmp(format, "html") == 0;
	}
	return false;
}

void sendResponse(uint16_t statusCode, const char *contentType, const char *content) {

	server.send(statusCode, contentType, content, strlen(content));
}

void getHttpResponseStart(char *htmlResponse, const char *titleSuffix = "") {

	strcpy(htmlResponse, "<html><head>");
	if (strcmp(titleSuffix, "") != 0) {
		strcat(htmlResponse, "<meta http-equiv=\"refresh\" content=\"");
		char tmp[4];
		if (strcmp(titleSuffix, "/action") == 0) {
			itoa(httpRedirectTimeout, tmp, 10);
			strcat(htmlResponse, tmp);
			strcat(htmlResponse, "; url=/");
		} else if (strcmp(titleSuffix, "/status") == 0) {
			itoa(httpRefreshInterval, tmp, 10);
			strcat(htmlResponse, tmp);
		}
		strcat(htmlResponse, "\"/>");
	}
	strcat(htmlResponse, "<title>");
	strcat(htmlResponse, httpTitle);
	strcat(htmlResponse, titleSuffix);
	strcat(htmlResponse, "</title></head><body>");
}

void getHttpResponseEnd(char *htmlResponse) {

	strcat(htmlResponse, "</body></html>");
}

void getHttpActionResponse(char *htmlResponse, const char *message) {

	getHttpResponseStart(htmlResponse, "/action");
	strcat(htmlResponse, "<pre>");
	strcat(htmlResponse, message);
	strcat(htmlResponse, "</pre><a href=\"/\">Go to index</a>");
	getHttpResponseEnd(htmlResponse);
}

void sendActionResponse(uint16_t statusCode, const char *responseMessage) {

	if (checkFormatIsHtml()) {
		char htmlResponse[256];
		getHttpActionResponse(htmlResponse, responseMessage);
		sendResponse(statusCode, "text/html", htmlResponse);
	} else {
		sendResponse(statusCode, "text/plain", responseMessage);
	}
}

void sendActionSuccessResponse(const char *successMessage) {

	sendActionResponse(200, successMessage);
}

int8_t getChannelParam() {

	if (!server.hasArg("channel")) {
		return 0;
	}

	char channelBuffer[httpMaxItemLength];
	getHttpParamValue(channelBuffer, "channel");

	uint8_t channelLength = strlen(channelBuffer);
	if (channelLength == 0) {
		return 0;
	} else if (channelLength > 3) {
		return -1;
	}

	uint8_t channelNumber = atoi(channelBuffer);

	if (!validateChannel(channelNumber)) {
		return -1;
	}

	return channelNumber;
}

void sendChannelPowerChanged(uint8_t channelNumber, const char *msgEnding) {

	char msg[33] = "Power of channel #";
	char channelLabel[4];
	itoa(channelNumber, channelLabel, 10);
	strcat(msg, channelLabel);
	strcat(msg, msgEnding);

	sendActionSuccessResponse(msg);
}

void sendPowerChangeError() {

	logMessage("Received invalid channel parameter for set power HTTP action");
	sendActionResponse(400, "Invalid argument: 'channel'");
}

void handleOn() {

	printHttpRequest();

	int8_t channel = getChannelParam();
	if (channel > 0) {
		switchOn(channel - 1);
		sendChannelPowerChanged(channel, " set to on");
	} else if (channel == 0) {
		switchOn();
		sendActionSuccessResponse("All channels power set to on");
	} else {
		sendPowerChangeError();
	}
}

void handleOff() {

	int8_t channel = getChannelParam();
	if (channel > 0) {
		switchOff(channel - 1);
		sendChannelPowerChanged(channel, " set to off");
	} else if (channel == 0) {
		switchOff();
		sendActionSuccessResponse("All channels power set to off");
	} else {
		sendPowerChangeError();
	}
}

void handleToggle() {

	int8_t channel = getChannelParam();
	if (channel > 0) {
		toggleRelay(channel - 1);
		sendChannelPowerChanged(channel, " toggled");
	} else if (channel == 0) {
		toggleRelay();
		sendActionSuccessResponse("All channels power toggled");
	} else {
		sendPowerChangeError();
	}
}

void getHttpStatusResponse(char *htmlResponse, const char *deviceStatus) {

	getHttpResponseStart(htmlResponse, "/status");
	strcat(htmlResponse, "<pre>");
	strcat(htmlResponse, deviceStatus);
	strcat(htmlResponse, "</pre><a href>Refresh</a> | <a href=\"/\">Go to index</a>");
	getHttpResponseEnd(htmlResponse);
}

void handleStatus() {

	printHttpRequest();
	char deviceStatus[420];
	getDeviceStatus(deviceStatus);
	if (checkFormatIsHtml()) {
		char htmlResponse[656];
		getHttpStatusResponse(htmlResponse, deviceStatus);
		sendResponse(200, "text/html", htmlResponse);
	} else {
		sendResponse(200, "application/json", deviceStatus);
	}
}

void handleEnableNoise() {

	printHttpRequest();
	enableNoise();
	sendActionSuccessResponse("Noise trigger enabled");
}

void handleDisableNoise() {

	printHttpRequest();
	disableNoise();
	sendActionSuccessResponse("Noise trigger disabled");
}

void handleEnableTimer() {

	printHttpRequest();
	enableTimer();
	sendActionSuccessResponse("Auto-off timer enabled");
}

void handleDisableTimer() {

	printHttpRequest();
	disableTimer();
	sendActionSuccessResponse("Auto-off timer disabled");
}

void sendTimeoutSet() {

	char msg[44] = "Auto-off timer timeout set to ";
	char tmp[11];
	getTimerTimeout(tmp);
	strcat(msg, tmp);
	strcat(msg, " ms");

	sendActionSuccessResponse(msg);
}

void handleSetTimer() {

	printHttpRequest();

	if (!server.hasArg("timeout")) {
		logMessage("Missing timeout parameter for '/set-timer' HTTP action");
		sendActionResponse(400, "Missing argument: 'timeout'");
		return;
	}

	char timeout[httpMaxItemLength];
	getHttpParamValue(timeout, "timeout");

	uint8_t timeoutLength = strlen(timeout);
	if (timeoutLength == 0 || timeoutLength > 10) {
		logMessage("Received invalid parameter for '/set-timer' HTTP action");
		sendActionResponse(400, "Invalid argument: 'timeout'");
		return;
	}

	setTimerTimeout(timeout);
	sendTimeoutSet();
}

void handleEnableMqtt() {

	printHttpRequest();
	enableMqtt();
	sendActionSuccessResponse("MQTT communication enabled");
}

void handleDisableMqtt() {

	printHttpRequest();
	disableMqtt();
	sendActionSuccessResponse("MQTT communication disabled");
}

void handleEnableOta() {

	printHttpRequest();
	enableOta();
	sendActionSuccessResponse("OTA update enabled");
}

void handleDisableOta() {

	printHttpRequest();
	disableOta();
	sendActionSuccessResponse("OTA update disabled");
}

void handleReset() {

	printHttpRequest();
	sendActionSuccessResponse("Restarting..");
	restartBoard();
}

void handleNotFound() {

	printHttpRequest();
	sendActionResponse(404, "Not found");
}

void getHttpRootSetPowerSection(char *htmlResponse, const char *action) {

	strcat(htmlResponse, " <a href=\"");
	strcat(htmlResponse, action);
	strcat(htmlResponse, "?format=html\">");
	strcat(htmlResponse, action);
	strcat(htmlResponse, "</a>");

	char channelValue[4];
	char channelLabel[5];
	for (uint8_t i = 0; i < channelsAvailable; i++) {
		itoa(i + 1, channelValue, 10);
		strcpy(channelLabel, "#");
		strcat(channelLabel, channelValue);

		strcat(htmlResponse, " <a href=\"");
		strcat(htmlResponse, action);
		strcat(htmlResponse, "?format=html&channel=");
		strcat(htmlResponse, channelValue);
		strcat(htmlResponse, "\">[ ");
		strcat(htmlResponse, channelLabel);
		strcat(htmlResponse, " ]</a>");
	}
}

void getHttpRootSetTimerSection(char *htmlResponse, const char *action) {

	const uint8_t timeoutsLength = 10;
	const char *timeoutLabels[timeoutsLength] = {
		"1m", "5m", "10m", "30m", "1h", "6h", "12h", "1d", "1w", "1mo"
	};
	const char *timeoutValues[timeoutsLength] = {
		"60000", "300000", "600000", "1800000", "3600000", "21600000", "43200000", "86400000", "604800000", "2628000000"
	};

	strcat(htmlResponse, action);

	char timeoutValue[11];
	char timeoutLabel[4];
	for (uint8_t i = 0; i < timeoutsLength; i++) {
		strcpy(timeoutLabel, timeoutLabels[i]);
		strcpy(timeoutValue, timeoutValues[i]);

		strcat(htmlResponse, " <a href=\"");
		strcat(htmlResponse, action);
		strcat(htmlResponse, "?format=html&timeout=");
		strcat(htmlResponse, timeoutValue);
		strcat(htmlResponse, "\">[ ");
		strcat(htmlResponse, timeoutLabel);
		strcat(htmlResponse, " ]</a>");
	}
}

void getHttpRootStatusSection(char *htmlResponse, const char *action) {

	strcat(htmlResponse, action);
	strcat(htmlResponse, " <a href=\"");
	strcat(htmlResponse, action);
	strcat(htmlResponse, "?format=html\">[ HTML ]</a>");
	strcat(htmlResponse, " <a href=\"");
	strcat(htmlResponse, action);
	strcat(htmlResponse, "?format=json\">[ JSON ]</a>");
}

void getHttpRootGenericSection(char *htmlResponse, const char *action) {

	strcat(htmlResponse, " <a href=\"");
	strcat(htmlResponse, action);
	strcat(htmlResponse, "?format=html\">");
	strcat(htmlResponse, action);
	strcat(htmlResponse, "</a>");
}

void getHttpRootResponse(char *htmlResponse) {

	getHttpResponseStart(htmlResponse);
	strcat(htmlResponse, "<h1>");
	strcat(htmlResponse, httpTitle);
	strcat(htmlResponse, "</h1><ul>");

	char action[16];
	for (uint8_t i = 0; i < actionsLength; i++) {
		strcpy(action, actions[i]);
		if (strcmp(action, "/enable-http") == 0 || strcmp(action, "/disable-http") == 0) {
			continue;
		}
		strcat(htmlResponse, "<li>");
		if (strcmp(action, "/on") == 0 || strcmp(action, "/off") == 0 || strcmp(action, "/toggle") == 0) {
			getHttpRootSetPowerSection(htmlResponse, action);
		} else if (strcmp(action, "/set-timer") == 0) {
			getHttpRootSetTimerSection(htmlResponse, action);
		} else if (strcmp(action, "/status") == 0) {
			getHttpRootStatusSection(htmlResponse, action);
		} else {
			getHttpRootGenericSection(htmlResponse, action);
		}
		strcat(htmlResponse, "</li>");
	}

	strcat(htmlResponse, "</ul>");
	getHttpResponseEnd(htmlResponse);
}

void handleRoot() {

	printHttpRequest();
	char htmlResponse[2000];
	getHttpRootResponse(htmlResponse);
	sendResponse(200, "text/html", htmlResponse);
}

void logHttpConnected() {

	char msg[34] = "HTTP server started on port ";
	char port[6];
	itoa(httpServerPort, port, 10);
	strcat(msg, port);
	logMessage(msg);
}

void httpConnect() {

	if (!httpEnabled) {
		return;
	}

	server.begin();
	logHttpConnected();
}

void httpDisconnect() {

	server.close();
	logMessage("HTTP server stopped");
}

void setHttpTitle() {

	strcpy(httpTitle, rootName);
	strcat(httpTitle, "-");
	strcat(httpTitle, commonName);
	strcat(httpTitle, "-");
	strcat(httpTitle, uniqueId);
}

void httpSetup() {

	logSerialMessage("\n--- HTTP setup ---");
	setHttpTitle();

	server.on("/", HTTP_GET, handleRoot);
	server.on("/on", HTTP_GET, handleOn);
	server.on("/off", HTTP_GET, handleOff);
	server.on("/toggle", HTTP_GET, handleToggle);
	server.on("/status", HTTP_GET, handleStatus);
	server.on("/enable-noise", HTTP_GET, handleEnableNoise);
	server.on("/disable-noise", HTTP_GET, handleDisableNoise);
	server.on("/enable-timer", HTTP_GET, handleEnableTimer);
	server.on("/disable-timer", HTTP_GET, handleDisableTimer);
	server.on("/set-timer", HTTP_GET, handleSetTimer);
	server.on("/enable-mqtt", HTTP_GET, handleEnableMqtt);
	server.on("/disable-mqtt", HTTP_GET, handleDisableMqtt);
	server.on("/enable-ota", HTTP_GET, handleEnableOta);
	server.on("/disable-ota", HTTP_GET, handleDisableOta);
	server.on("/reset", HTTP_GET, handleReset);

	server.onNotFound(handleNotFound);

	httpConnect();
}

void evalHttpStatus(uint32_t currEvalTime) {

	if (!httpEnabled || !getWifiStatus()) {
		return;
	}

	if ((uint32_t)(currEvalTime - lastHttpEvalTime) < httpEvalTimeout) {
		return;
	}
	lastHttpEvalTime = currEvalTime;

	server.handleClient();
}

void enableHttp() {

	if (httpEnabled) {
		return;
	}

	httpEnabled = true;
	logMessage("HTTP communication enabled");
	httpConnect();
}

void disableHttp() {

	if (!httpEnabled) {
		return;
	}

	httpEnabled = false;
	logMessage("HTTP communication disabled");
	httpDisconnect();
}

bool getHttpEnabled() {

	return httpEnabled;
}
