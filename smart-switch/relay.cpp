#include "relay.hpp"

bool relayStatus = false;

void relaySetup() {

  logSerialMessage("\n--- Relay setup ---");

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // HIGH/LOW are inverted on NodeMCU

  logSerialMessage("Managing relay state");
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
