#include "timer.hpp"

uint32_t timerTimeout = timerTimeoutDefault;
uint32_t timerStartTime = 0;
bool timerEnabled = true;
bool timerRunning = false;

void getTimerTimeout(char *timeoutBuffer) {

	snprintf(timeoutBuffer, 11, "%lu", timerTimeout);
}

void logTimerSetupEnd() {

	char msg[36] = "Auto-off timer set to ";
	char tmp[11];
	getTimerTimeout(tmp);
	strcat(msg, tmp);
	strcat(msg, " ms");

	logSerialMessage(msg);
}

void timerSetup() {

	logSerialMessage("\n--- Timer setup ---");

	if (timerEnabled) {
		logTimerSetupEnd();
	}
}

void logTimerStart() {

	char msg[45] = "Auto-off timer will trigger in ";
	char tmp[11];
	getTimerTimeout(tmp);
	strcat(msg, tmp);
	strcat(msg, " ms");

	logMessage(msg);
}

void startTimer(uint32_t startTime) {

	timerStartTime = startTime;
	timerRunning = true;
	logTimerStart();
}

void clearTimer() {

	timerStartTime = 0;
	timerRunning = false;
}

void enableTimer() {

	if (timerEnabled) {
		return;
	}

	timerEnabled = true;
	logMessage("Auto-off timer enabled");
}

void disableTimer() {

	if (!timerEnabled) {
		return;
	}

	timerEnabled = false;
	clearTimer();
	logMessage("Auto-off timer disabled");
}

void triggerTimer() {

	clearTimer();
	logMessage("Auto-off timer triggered");
	switchOff();
}

uint32_t getTimerElapsedTime() {

	if (!timerRunning) {
		return 0;
	}

	return (uint32_t)(millis() - timerStartTime);
}

void evalTimerStatus(uint32_t currEvalTime) {

	if (!timerEnabled) {
		return;
	}

	if (!getPowerStatus()) {
		if (timerRunning) {
			clearTimer();
			logMessage("Auto-off timer cleared");
		}
		return;
	}

	if (!timerRunning) {
		startTimer(currEvalTime);
	}

	if ((uint32_t)(currEvalTime - timerStartTime) < timerTimeout) {
		return;
	}

	triggerTimer();
}

void logTimerTimeoutSet() {

	char msg[45] = "Auto-off timer timeout set to ";
	char tmp[11];
	getTimerTimeout(tmp);
	strcat(msg, tmp);
	strcat(msg, " ms");

	logMessage(msg);
}

void setTimerTimeout(uint32_t timeout) {

	timerTimeout = timeout;
	logTimerTimeoutSet();
}

void setTimerTimeout(const char *timeout) {

	uint32_t timeoutNum = strtoul(timeout, 0, 10);
	if (timeoutNum != 0) {
		setTimerTimeout(timeoutNum);
	} else {
		logMessage("Tried to set invalid timeout value");
	}
}

uint32_t getTimerTimeout() {

	return timerTimeout;
}

bool getTimerEnabled() {

	return timerEnabled;
}
