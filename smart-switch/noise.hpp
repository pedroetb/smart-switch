#ifndef SMART_SWITCH_NOISE_H
#define SMART_SWITCH_NOISE_H

#include "config.hpp"
#include "common.hpp"
#include "relay.hpp"

void noiseSetup();
void evalNoiseStatus(uint32_t currEvalTime);
void enableNoise(uint8_t index);
void enableNoise();
void disableNoise(uint8_t index);
void disableNoise();
bool getNoiseEnabled(uint8_t index);
bool getNoiseValue();

#endif
