#include "relay.hpp"

bool relayStatus[channelsAvailable] = {};

bool getRelayStatus(uint8_t index) {

	return relayStatus[index];
}

void writeRelayStatus(uint8_t index) {

	digitalWrite(relayPin[index], !getRelayStatus(index)); // HIGH/LOW are inverted on NodeMCU
}

void relaySetup() {

	logSerialMessage("\n--- Relay setup ---");

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		pinMode(relayPin[i], OUTPUT);
		writeRelayStatus(i);
	}

	logSerialMessage("Managing relays state");
}

void setRelayStatus(uint8_t index, bool status) {

	relayStatus[index] = status;
}

void logApplyRelayStatus(uint8_t index) {

	char msg[37] = "Relay status in channel #";
	char tmp[4];
	itoa(index + 1, tmp, 10);
	strcat(msg, tmp);
	strcat(msg, " is: ");

	if (getRelayStatus(index)) {
		strcat(msg, "ON");
	} else {
		strcat(msg, "OFF");
	}

	logMessage(msg);
}

void applyRelayStatus(uint8_t index) {

	logApplyRelayStatus(index);
	writeRelayStatus(index);
}

void enableRelay(uint8_t index) {

	setRelayStatus(index, true);
	applyRelayStatus(index);
}

void disableRelay(uint8_t index) {

	setRelayStatus(index, false);
	applyRelayStatus(index);
}

void toggleRelay(uint8_t index) {

	setRelayStatus(index, !getRelayStatus(index));
	applyRelayStatus(index);
}

void toggleRelay() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		toggleRelay(i);
	}
}

void switchOn(uint8_t index) {

	bool powerStatus = getPowerStatus(index);
	if (!powerStatus) {
		toggleRelay(index);
	}
}

void switchOn() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		switchOn(i);
	}
}

void switchOff(uint8_t index) {

	bool powerStatus = getPowerStatus(index);
	if (powerStatus) {
		toggleRelay(index);
	}
}

void switchOff() {

	for (uint8_t i = 0; i < channelsAvailable; i++) {
		switchOff(i);
	}
}
