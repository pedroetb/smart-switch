#include "noise.hpp"

uint32_t lastNoiseTime = 0;
uint32_t lastAcceptedNoiseTime = 0;
bool noiseEnabled = true;
bool lastNoiseValue = 0;
bool noiseValue = 0;

void noiseSetup() {

  pinMode(noisePin, INPUT);
}

void evalNoiseStatus(uint32_t currNoiseTime) {

  if (!noiseEnabled) {
    return;
  }

  noiseValue = digitalRead(noisePin);

  if (noiseValue == 0) {
    lastNoiseValue = noiseValue;
    return;
  }

  bool silenceInPreviousEval = lastNoiseValue == 0;

  if (silenceInPreviousEval) {
    bool noiseIsDiscontinuous = currNoiseTime - noiseLongDebounceTimeout > lastNoiseTime;
    bool noiseIsNotOverlapped = currNoiseTime - noiseRepetitiveDebounceTimeout < lastNoiseTime;
    bool lastNoisePatternFinished = currNoiseTime - noisePatternDebounceTimeout > lastAcceptedNoiseTime;
  
    if (noiseIsDiscontinuous && noiseIsNotOverlapped && lastNoisePatternFinished) {
      logMessage("Noise (double clap) detected");
      toggleRelay();
      lastAcceptedNoiseTime = currNoiseTime;
    }
  }

  lastNoiseTime = currNoiseTime;
  lastNoiseValue = noiseValue;
}

void enableNoise() {

  if (noiseEnabled) {
    return;
  }

  noiseEnabled = true;
  logMessage("Noise trigger enabled");
}

void disableNoise() {

  if (!noiseEnabled) {
    return;
  }

  noiseEnabled = false;
  logMessage("Noise trigger disabled");
}

bool getNoiseEnabled() {

  return noiseEnabled;
}

bool getNoiseValue() {

  return noiseValue;
}
