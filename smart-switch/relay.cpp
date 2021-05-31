#include "relay.hpp"

bool relayStatus = false;

void writeRelayStatus() {

	digitalWrite(relayPin, !relayStatus); // HIGH/LOW are inverted on NodeMCU
}

void relaySetup() {

	logSerialMessage("\n--- Relay setup ---");

	pinMode(relayPin, OUTPUT);
	writeRelayStatus();

	logSerialMessage("Managing relay state");
}

bool getRelayStatus() {

	return relayStatus;
}

void logApplyRelayStatus() {

	char msg[14] = "Relay is: ";

	if (relayStatus) {
		strcat(msg, "ON");
	} else {
		strcat(msg, "OFF");
	}

	logMessage(msg);
}

void applyRelayStatus() {

	logApplyRelayStatus();
	writeRelayStatus();
}

void enableRelay() {

	relayStatus = true;
	applyRelayStatus();
}

void disableRelay() {

	relayStatus = false;
	applyRelayStatus();
}

void toggleRelay() {

	relayStatus = !relayStatus;
	applyRelayStatus();
}

void switchOn() {

	bool powerStatus = getMeasurePowerStatus();
	if (!powerStatus) {
		toggleRelay();
	}
}

void switchOff() {

	bool powerStatus = getMeasurePowerStatus();
	if (powerStatus) {
		toggleRelay();
	}
}
