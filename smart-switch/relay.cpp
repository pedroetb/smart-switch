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

void applyRelayStatus() {

  logMessage("Relay is: " + (String)(relayStatus ? "ON" : "OFF"));
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
