#include "relay.hpp"

bool relayStatus = false;

void relaySetup() {

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // HIGH/LOW are inverted on NodeMCU
}

bool getRelayStatus() {

  return relayStatus;
}

void applyRelayStatus() {

  logMessage("Relay is: " + (String)(relayStatus ? "ON" : "OFF"));
  digitalWrite(relayPin, !relayStatus); // HIGH/LOW are inverted on NodeMCU
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
