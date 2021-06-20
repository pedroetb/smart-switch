#include "timer.hpp"

uint32_t timerTimeout = timerTimeoutDefault;
uint32_t timerStartTime = 0;
bool timerEnabled[channelsAvailable];
bool timerRunning = false;

bool getTimerEnabled(const uint8_t index) {

	return timerEnabled[index];
}

bool getTimerEnabled() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		if (getTimerEnabled(i)) {
			return true;
		}
	}
	return false;
}

void getTimerTimeout(char *timeoutBuffer) {

	snprintf(timeoutBuffer, maxParamSize, "%lu", timerTimeout);
}

void logTimerSetupEnd() {

	char msg[maxParamSize + 25] = "Auto-off timer set to ";
	char tmp[maxParamSize];
	getTimerTimeout(tmp);
	strcat(msg, tmp);
	strcat(msg, " ms");

	logSerialMessage(msg);
}

void timerSetup() {

	logSerialMessage("\n--- Timer setup ---");

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		timerEnabled[i] = true;
	}

	if (getTimerEnabled()) {
		logTimerSetupEnd();
	}
}

void logTimerStart() {

	char msg[maxParamSize + 34] = "Auto-off timer will trigger in ";
	char tmp[maxParamSize];
	getTimerTimeout(tmp);
	strcat(msg, tmp);
	strcat(msg, " ms");

	logMessage(msg);
}

void startTimer(const uint32_t startTime) {

	timerStartTime = startTime;
	timerRunning = true;
	logTimerStart();
}

void clearTimer() {

	timerStartTime = 0;
	timerRunning = false;
}

void enableTimer(const uint8_t index) {

	if (timerEnabled[index]) {
		return;
	}

	timerEnabled[index] = true;

	char msg[38] = "Auto-off timer enabled in channel #";
	char tmp[4];
	itoa(index + 1, tmp, 10);
	strcat(msg, tmp);
	logMessage(msg);
}

void enableTimer() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		enableTimer(i);
	}
}

void disableTimer(const uint8_t index) {

	if (!timerEnabled[index]) {
		return;
	}

	timerEnabled[index] = false;
	if (!getTimerEnabled()) {
		clearTimer();
	}

	char msg[38] = "Auto-off timer disabled in channel #";
	char tmp[4];
	itoa(index + 1, tmp, 10);
	strcat(msg, tmp);
	logMessage(msg);
}

void disableTimer() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		disableTimer(i);
	}
}

void triggerTimer() {

	clearTimer();
	logMessage("Auto-off timer triggered");
	for (uint8_t i = 0; i < channelsAvailable; i++) {
		if (timerEnabled[i]) {
			switchOff(i);
		}
	}
}

uint32_t getTimerElapsedTime() {

	if (!timerRunning) {
		return 0;
	}

	return millis() - timerStartTime;
}

bool timerIsUseless() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		if (timerEnabled[i] && getPowerStatus(i)) {
			return false;
		}
	}
	return true;
}

void cancelTimer() {

	if (!timerRunning) {
		return;
	}
	clearTimer();
	logMessage("Auto-off timer cancelled");
}

void evalTimerStatus(const uint32_t currEvalTime) {

	if (!getTimerEnabled()) {
		return;
	}

	if (!getPowerStatus() || timerIsUseless()) {
		cancelTimer();
		return;
	}

	if (!timerRunning) {
		startTimer(currEvalTime);
	}

	if ((currEvalTime - timerStartTime) < timerTimeout) {
		return;
	}

	triggerTimer();
}

void logTimerTimeoutSet() {

	char msg[maxParamSize + 33] = "Auto-off timer timeout set to ";
	char tmp[maxParamSize];
	getTimerTimeout(tmp);
	strcat(msg, tmp);
	strcat(msg, " ms");

	logMessage(msg);
}

void setTimerTimeout(const uint32_t timeout) {

	timerTimeout = timeout;
	logTimerTimeoutSet();
}

void setTimerTimeout(const char *timeout) {

	const uint32_t timeoutNum = strtoul(timeout, 0, 10);
	if (timeoutNum != 0) {
		setTimerTimeout(timeoutNum);
	} else {
		logMessage("Tried to set invalid timeout value");
	}
}

uint32_t getTimerTimeout() {

	return timerTimeout;
}
