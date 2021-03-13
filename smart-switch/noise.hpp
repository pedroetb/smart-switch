#ifndef SMART_SWITCH_NOISE_H
#define SMART_SWITCH_NOISE_H

#include "config.hpp"
#include "common.hpp"
#include "relay.hpp"

void noiseSetup();
void evalNoiseStatus(uint32_t currNoiseTime);
void enableNoise();
void disableNoise();
bool getNoiseEnabled();
bool getNoiseValue();

#endif
