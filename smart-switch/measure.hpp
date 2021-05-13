#ifndef SMART_SWITCH_MEASURE_H
#define SMART_SWITCH_MEASURE_H

#include "config.hpp"
#include "common.hpp"

void measureSetup();
void evalMeasureStatus(uint32_t currEvalTime);
bool getMeasurePowerStatus();
uint8_t getMeasureFrequency();

#endif
