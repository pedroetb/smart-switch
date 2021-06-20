#ifndef SMART_SWITCH_MEASURE_H
#define SMART_SWITCH_MEASURE_H

#include "config.hpp"
#include "common.hpp"

constexpr uint16_t measureFrequencyDuration = 1000;
constexpr uint8_t measureFrequencyDivisor = 2;

void measureSetup();
void evalMeasureStatus(const uint32_t currEvalTime);
bool getPowerStatus(const uint8_t index);
bool getPowerStatus();
uint8_t getNetFrequency(const uint8_t index);

#endif
