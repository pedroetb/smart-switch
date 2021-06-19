#ifndef SMART_SWITCH_TIMER_H
#define SMART_SWITCH_TIMER_H

#include "config.hpp"
#include "common.hpp"
#include "relay.hpp"
#include "measure.hpp"

void timerSetup();
void evalTimerStatus(const uint32_t currEvalTime);
void enableTimer(const uint8_t index);
void enableTimer();
void disableTimer(const uint8_t index);
void disableTimer();
uint32_t getTimerElapsedTime();
void setTimerTimeout(const uint32_t timeout);
void setTimerTimeout(const char *timeout);
uint32_t getTimerTimeout();
void getTimerTimeout(char *timeoutBuffer);
bool getTimerEnabled(const uint8_t index);
bool getTimerEnabled();

#endif
