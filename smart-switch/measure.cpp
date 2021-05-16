#include "measure.hpp"

bool measurePowerStatus = false;
bool lastMeasurePowerStatus = false;
bool measuringFrequency = false;
uint8_t zeroCrossingCounter = 0;
uint8_t lastFrequencyMeasured = 0;
uint32_t lastFrequencyMeasureEndTime = 0;
uint32_t lastFrequencyMeasureStartTime = 0;
uint32_t lastZeroCrossingTime = 0;

void ICACHE_RAM_ATTR zeroCrossingCallback() {

	if (measuringFrequency) {
		zeroCrossingCounter++;
	}

	lastZeroCrossingTime = millis();
	measurePowerStatus = true;
}

void measureSetup() {

	logSerialMessage("\n--- Measure setup ---");

	pinMode(measurePin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(measurePin), zeroCrossingCallback, RISING);

	logSerialMessage("Listening signal zero-crossing events");
}

void handleMeasurePowerStatus(uint32_t currTime) {

	if (measurePowerStatus && lastZeroCrossingTime > 0 && lastZeroCrossingTime < currTime &&
		(uint32_t)(currTime - lastZeroCrossingTime) >= resetMeasurePowerStatusTimeout) {

		measurePowerStatus = false;
		lastZeroCrossingTime = 0;
	}

	if (lastMeasurePowerStatus != measurePowerStatus) {
		lastMeasurePowerStatus = measurePowerStatus;
		logMessage("Measure power status is: " + (String)(measurePowerStatus ? "ON" : "OFF"));
	}
}

void onMeasurementStart(uint32_t currTime) {

	measuringFrequency = true;
	lastFrequencyMeasureStartTime = currTime;
}

void onMeasurementDone(uint32_t currTime) {

	measuringFrequency = false;
	lastFrequencyMeasureEndTime = currTime;
	lastFrequencyMeasured = zeroCrossingCounter / measureFrequencyDivisor;
	zeroCrossingCounter = 0;
	lastFrequencyMeasureStartTime = 0;
}

void measureFrequency(uint32_t currTime) {

	if (measuringFrequency) {
		if (currTime - lastFrequencyMeasureStartTime >= measureFrequencyDuration) {
			onMeasurementDone(currTime);
		}

	} else if ((uint32_t)(currTime - lastFrequencyMeasureEndTime) >= measureFrequencyInterval) {
		onMeasurementStart(currTime);
	}
}

void evalMeasureStatus(uint32_t currEvalTime) {

	handleMeasurePowerStatus(currEvalTime);
	measureFrequency(millis());
}

bool getMeasurePowerStatus() {

	return measurePowerStatus;
}

uint8_t getMeasureFrequency() {

	return lastFrequencyMeasured;
}
