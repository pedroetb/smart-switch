#include "measure.hpp"

bool powerStatus[channelsAvailable];
bool lastPowerStatus[channelsAvailable];
bool measuringFrequency = false;
uint8_t zeroCrossingCounter[channelsAvailable];
uint8_t lastFrequencyMeasured[channelsAvailable];
uint32_t lastFrequencyMeasureEndTime = 0;
uint32_t lastFrequencyMeasureStartTime = 0;
uint32_t lastZeroCrossingTime[channelsAvailable];

template<uint8_t index>
inline void ICACHE_RAM_ATTR zeroCrossingCallback() {

	if (measuringFrequency) {
		zeroCrossingCounter[index]++;
	}

	lastZeroCrossingTime[index] = millis();
	powerStatus[index] = true;
}

template<uint8_t index>
void listenZeroCrossing() {

	attachInterrupt(digitalPinToInterrupt(measurePin[index]), zeroCrossingCallback<index>, RISING);
	if (index > 0) {
		listenZeroCrossing<index - 1>();
	}
}

void measureSetup() {

	logSerialMessage("\n--- Measure setup ---");

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		powerStatus[i] = false;
		lastPowerStatus[i] = false;
		zeroCrossingCounter[i] = 0;
		lastFrequencyMeasured[i] = 0;
		lastZeroCrossingTime[i] = 0;
		pinMode(measurePin[i], INPUT_PULLUP);
	}

	listenZeroCrossing<channelsAvailable - 1>();

	logSerialMessage("Listening signal zero-crossing events");
}

void logMeasureChange(const uint8_t index) {

	char msg[37] = "Power status in channel #";
	char tmp[4];
	itoa(index + 1, tmp, 10);
	strcat(msg, tmp);
	strcat(msg, " is: ");

	if (powerStatus[index]) {
		strcat(msg, "ON");
	} else {
		strcat(msg, "OFF");
	}

	logMessage(msg);
}

void handlePowerStatus(const uint8_t index, const uint32_t currTime) {

	const uint32_t lastPowerDetectedTime = lastZeroCrossingTime[index];
	const bool powerWasOnRecently = powerStatus[index];
	const bool powerDetectedBeforeThisIteration = lastPowerDetectedTime > 0 && lastPowerDetectedTime < currTime;
	const bool powerLossDetectionThresholdReached = (currTime - lastPowerDetectedTime) >= resetPowerStatusTimeout;

	if (powerWasOnRecently && powerDetectedBeforeThisIteration && powerLossDetectionThresholdReached) {
		powerStatus[index] = false;
		lastZeroCrossingTime[index] = 0;
	}

	if (lastPowerStatus[index] != powerStatus[index]) {
		lastPowerStatus[index] = powerStatus[index];
		logMeasureChange(index);
	}
}

void onMeasurementStart(const uint32_t currTime) {

	measuringFrequency = true;
	lastFrequencyMeasureStartTime = currTime;
}

void onMeasurementDone(const uint32_t currTime) {

	measuringFrequency = false;
	lastFrequencyMeasureEndTime = currTime;

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		lastFrequencyMeasured[i] = zeroCrossingCounter[i] / measureFrequencyDivisor;
		zeroCrossingCounter[i] = 0;
	}

	lastFrequencyMeasureStartTime = 0;
}

void measureFrequency(const uint32_t currTime) {

	if (measuringFrequency) {
		if ((currTime - lastFrequencyMeasureStartTime) >= measureFrequencyDuration) {
			onMeasurementDone(currTime);
		}
	} else if ((currTime - lastFrequencyMeasureEndTime) >= measureFrequencyInterval) {
		onMeasurementStart(currTime);
	}
}

void evalMeasureStatus(const uint32_t currEvalTime) {

	measureFrequency(currEvalTime);

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		handlePowerStatus(i, currEvalTime);
	}
}

bool getPowerStatus(const uint8_t index) {

	return powerStatus[index];
}

bool getPowerStatus() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		if (getPowerStatus(i)) {
			return true;
		}
	}
	return false;
}

uint8_t getNetFrequency(const uint8_t index) {

	return lastFrequencyMeasured[index];
}
