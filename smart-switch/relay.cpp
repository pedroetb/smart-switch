#include "relay.hpp"

bool relayStatus[channelsAvailable];

void writeRelayStatus(const uint8_t index) {

	digitalWrite(relayPin[index], !relayStatus[index]); // HIGH/LOW are inverted on NodeMCU
}

void relaySetup() {

	logSerialMessage("\n--- Relay setup ---");

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		relayStatus[i] = false;
		pinMode(relayPin[i], OUTPUT);
		writeRelayStatus(i);
	}

	logSerialMessage("Managing relays state");
}

bool getRelayStatus(const uint8_t index) {

	return relayStatus[index];
}

void logApplyRelayStatus(const uint8_t index) {

	char msg[37] = "Relay status in channel #";
	char tmp[4];
	itoa(index + 1, tmp, 10);
	strcat(msg, tmp);
	strcat(msg, " is: ");

	if (relayStatus[index]) {
		strcat(msg, "ON");
	} else {
		strcat(msg, "OFF");
	}

	logMessage(msg);
}

void applyRelayStatus(const uint8_t index) {

	logApplyRelayStatus(index);
	writeRelayStatus(index);
}

void enableRelay(const uint8_t index) {

	relayStatus[index] = true;
	applyRelayStatus(index);
}

void disableRelay(const uint8_t index) {

	relayStatus[index] = false;
	applyRelayStatus(index);
}

void toggleRelay(const uint8_t index) {

	relayStatus[index] = !relayStatus[index];
	applyRelayStatus(index);
}

void toggleRelay() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		toggleRelay(i);
	}
}

void switchOn(const uint8_t index) {

	const bool powerStatus = getPowerStatus(index);
	if (!powerStatus) {
		toggleRelay(index);
	}
}

void switchOn() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		switchOn(i);
	}
}

void switchOff(const uint8_t index) {

	const bool powerStatus = getPowerStatus(index);
	if (powerStatus) {
		toggleRelay(index);
	}
}

void switchOff() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		switchOff(i);
	}
}
