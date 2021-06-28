#include "noise.hpp"

uint32_t lastNoiseTime = 0;
uint32_t lastAcceptedNoiseTime = 0;
bool noiseEnabled[channelsAvailable];
bool lastNoiseValue = 0;
bool noiseValue = 0;

void noiseSetup() {

	logSerialMessage("\n--- Noise setup ---");

	pinMode(noisePin, INPUT);

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		noiseEnabled[i] = noiseEnabledDefault;
	}

	logSerialMessage("Listening double-clap sound patterns");
}

bool getNoiseEnabled(const uint8_t index) {

	return noiseEnabled[index];
}

bool getNoiseEnabled() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		if (noiseEnabled[i]) {
			return true;
		}
	}
	return false;
}

void onNoiseTrigger(const uint32_t currEvalTime) {

	logMessage("Noise (double clap) detected");

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		if (noiseEnabled[i]) {
			toggleRelay(i);
		}
	}

	lastAcceptedNoiseTime = currEvalTime;
}

void evalNoiseStatus(const uint32_t currEvalTime) {

	if (!getNoiseEnabled()) {
		return;
	}

	noiseValue = !digitalRead(noisePin);

	if (noiseValue == 0) {
		lastNoiseValue = noiseValue;
		return;
	}

	const bool silenceInPreviousEval = lastNoiseValue == 0;

	if (silenceInPreviousEval) {
		const bool noiseIsDiscontinuous = currEvalTime - noiseLongDebounceTimeout > lastNoiseTime;
		const bool noiseIsNotOverlapped = currEvalTime - noiseRepetitiveDebounceTimeout < lastNoiseTime;
		const bool lastNoisePatternFinished = currEvalTime - noisePatternDebounceTimeout > lastAcceptedNoiseTime;

		if (noiseIsDiscontinuous && noiseIsNotOverlapped && lastNoisePatternFinished) {
			onNoiseTrigger(currEvalTime);
		}
	}

	lastNoiseTime = currEvalTime;
	lastNoiseValue = noiseValue;
}

void enableNoise(const uint8_t index) {

	if (noiseEnabled[index]) {
		return;
	}

	noiseEnabled[index] = true;

	char msg[38] = "Noise trigger enabled in channel #";
	char tmp[4];
	itoa(index + 1, tmp, 10);
	strcat(msg, tmp);
	logMessage(msg);
}

void enableNoise() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		enableNoise(i);
	}
}

void disableNoise(const uint8_t index) {

	if (!noiseEnabled[index]) {
		return;
	}

	noiseEnabled[index] = false;

	char msg[39] = "Noise trigger disabled in channel #";
	char tmp[4];
	itoa(index + 1, tmp, 10);
	strcat(msg, tmp);
	logMessage(msg);
}

void disableNoise() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		disableNoise(i);
	}
}

bool getNoiseValue() {

	return noiseValue;
}
