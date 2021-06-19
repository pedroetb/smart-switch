#ifndef SMART_SWITCH_MEASURE_H
#define SMART_SWITCH_MEASURE_H

#include "config.hpp"
#include "common.hpp"

void measureSetup();
void evalMeasureStatus(const uint32_t currEvalTime);
bool getPowerStatus(const uint8_t index);
bool getPowerStatus();
uint8_t getNetFrequency(const uint8_t index);

#endif
