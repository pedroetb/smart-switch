#include "config.hpp"
#include "common.hpp"
#include "relay.hpp"
#include "measure.hpp"
#include "timer.hpp"
#include "noise.hpp"
#include "wifi.hpp"
#include "http.hpp"
#include "mqtt.hpp"
#include "ota.hpp"

void logSetupEnd() {

	char msg[39] = "\n--- Setup completed in ";
	char tmp[6];
	ltoa(millis(), tmp, 10);
	strcat(msg, tmp);
	strcat(msg, " ms ---\n\n");
	logSerialMessage(msg);
}

void setup() {

	commonSetup();
	relaySetup();
	timerSetup();
	noiseSetup();
	wifiSetup();
	httpSetup();
	mqttSetup();
	otaSetup();
	measureSetup();

	logSetupEnd();
}

void loop() {

	const uint32_t currEvalTime = millis();

	evalMeasureStatus(currEvalTime);
	evalTimerStatus(currEvalTime);
	evalNoiseStatus(currEvalTime);
	evalWifiStatus(currEvalTime);
	evalHttpStatus(currEvalTime);
	evalMqttStatus(currEvalTime);
	evalOtaStatus(currEvalTime);
}
