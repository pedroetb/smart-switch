#ifndef SMART_SWITCH_TIMER_H
#define SMART_SWITCH_TIMER_H

#include "config.hpp"
#include "common.hpp"
#include "relay.hpp"
#include "measure.hpp"

void timerSetup();
void evalTimerStatus(uint32_t currEvalTime);
void enableTimer();
void disableTimer();
uint32_t getTimerElapsedTime();
void setTimerTimeout(uint32_t timeout);
void setTimerTimeout(String timeoutStr);
uint32_t getTimerTimeout();
bool getTimerEnabled();

#endif
