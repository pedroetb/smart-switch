#include "timer.hpp"

uint32_t timerTimeout = timerTimeoutDefault;
uint32_t timerStartTime = 0;
bool timerEnabled = true;
bool timerRunning = false;

void startTimer() {

  if (timerEnabled) {
    timerStartTime = millis();
    timerRunning = true;
    logMessage("Auto-off timer will trigger in " + (String)timerTimeout + " ms");
  }
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
  disableRelay();
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

  if (!getRelayStatus()) {
    if (timerRunning) {
      clearTimer();
      logMessage("Auto-off timer cleared");
    }
    return;
  }

  if (!timerRunning) {
    startTimer();
  }

  if ((uint32_t)(currEvalTime - timerStartTime) < timerTimeout) {
    return;
  }

  triggerTimer();
}

void setTimerTimeout(uint32_t timeout) {

  timerTimeout = timeout;
  logMessage("Auto-off timer timeout set to " + (String)timeout + " ms");
}

void setTimerTimeout(String timeoutStr) {

  uint32_t timeoutNum = strtoul(timeoutStr.c_str(), 0, 10);
  setTimerTimeout(timeoutNum);
}

uint32_t getTimerTimeout() {

  return timerTimeout;
}

bool getTimerEnabled() {

  return timerEnabled;
}
