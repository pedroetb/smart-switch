#include "http.hpp"

ESP8266WebServer server(httpServerPort);
String httpTitle;
uint32_t lastHttpEvalTime = 0;
bool httpEnabled = true;

void printHttpRequest() {

  logMessage("HTTP received uri: " + server.uri());
  uint8_t argsCount = server.args();
  for (uint8_t i = 0; i < argsCount; i++) {
    logMessage("HTTP received arg: " + server.argName(i) + "=" + server.arg(i));
  }
}

bool checkFormatIsHtml() {

  if (server.hasArg("format")) {
    String statusFormat = server.arg("format");
    return statusFormat.equals("html");
  }
  return false;
}

String getHttpActionResponse(String message) {

  return
    "<html>"
      "<head>"
        "<meta http-equiv=\"refresh\" content=\"" + (String)httpRedirectTimeout + "; url=/\"/>"
        "<title>" +
          httpTitle + "/action"
        "</title>"
      "</head>"
      "<body>"
        "<pre>" +
          message +
        "</pre>"
        "<a href=\"/\">"
          "Go to index"
        "</a>"
      "</body>"
    "</html>";
}

void sendResponse(uint16_t statusCode, String responseMessage) {

  if (checkFormatIsHtml()) {
    server.send(statusCode, "text/html", getHttpActionResponse(responseMessage));
  } else {
    server.send(statusCode, "text/plain", responseMessage);
  }
}

void sendSuccessResponse(String successMessage) {

  sendResponse(200, successMessage);
}

void handleOn() {

  printHttpRequest();
  enableRelay();
  sendSuccessResponse("Relay set to on");
}

void handleOff() {

  printHttpRequest();
  disableRelay();
  sendSuccessResponse("Relay set to off");
}

void handleToggle() {

  printHttpRequest();
  toggleRelay();
  sendSuccessResponse("Relay toggled");
}

String getHttpStatusResponse(String deviceStatus) {

  return
    "<html>"
      "<head>"
        "<meta http-equiv=\"refresh\" content=\"" + (String)httpRefreshInterval + "\"/>"
        "<title>" +
          httpTitle + "/status"
        "</title>"
      "</head>"
      "<body>"
        "<pre>" +
          deviceStatus +
        "</pre>"
        "<a href>"
          "Refresh"
        "</a>"
        " | "
        "<a href=\"/\">"
          "Go to index"
        "</a>"
      "</body>"
    "</html>";
}

void handleStatus() {

  printHttpRequest();
  String deviceStatus = getDeviceStatus();
  if (checkFormatIsHtml()) {
    server.send(200, "text/html", getHttpStatusResponse(deviceStatus));
  } else {
    server.send(200, "application/json", deviceStatus);
  }
}

void handleEnableNoise() {

  printHttpRequest();
  enableNoise();
  sendSuccessResponse("Noise trigger enabled");
}

void handleDisableNoise() {

  printHttpRequest();
  disableNoise();
  sendSuccessResponse("Noise trigger disabled");
}

void handleEnableTimer() {

  printHttpRequest();
  enableTimer();
  sendSuccessResponse("Auto-off timer enabled");
}

void handleDisableTimer() {

  printHttpRequest();
  disableTimer();
  sendSuccessResponse("Auto-off timer disabled");
}

void handleSetTimer() {

  printHttpRequest();
  if (server.hasArg("timeout")) {
    setTimerTimeout(server.arg("timeout"));
    sendSuccessResponse("Auto-off timer timeout set to " + (String)getTimerTimeout() + " ms");
  } else {
    sendResponse(400, "Missing argument: 'timeout'");
  }
}

void handleEnableMqtt() {

  printHttpRequest();
  enableMqtt();
  sendSuccessResponse("MQTT communication enabled");
}

void handleDisableMqtt() {

  printHttpRequest();
  disableMqtt();
  sendSuccessResponse("MQTT communication disabled");
}

void handleEnableOta() {

  printHttpRequest();
  enableOta();
  sendSuccessResponse("OTA update enabled");
}

void handleDisableOta() {

  printHttpRequest();
  disableOta();
  sendSuccessResponse("OTA update disabled");
}

void handleReset() {

  printHttpRequest();
  server.send(200, "text/plain", "Restarting..");
  restartBoard();
}

void handleNotFound() {

  printHttpRequest();
  sendResponse(404, "Not found");
}

String getHttpRootResponse() {

  String res =
    "<html>"
      "<head>"
        "<title>" +
          httpTitle +
        "</title>"
      "</head>"
      "<body>"
        "<h1>" +
          httpTitle +
        "</h1>"
        "<ul>";

  for (uint8_t i = 0; i < actionsLength; i++) {
    String action = actions[i];
    if (action.equals("/enable-http") || action.equals("/disable-http")) {
      continue;
    }
    res +=
          "<li>";
    if (action.equals("/set-timer")) {
      res +=
              action +
              " <a href=\"" + action + "?format=html&timeout=60000\">"
                "[ 1m ]"
              "</a>"
              " <a href=\"" + action + "?format=html&timeout=300000\">"
                "[ 5m ]"
              "</a>"
              " <a href=\"" + action + "?format=html&timeout=600000\">"
                "[ 10m ]"
              "</a>"
              " <a href=\"" + action + "?format=html&timeout=1800000\">"
                "[ 30m ]"
              "</a>"
              " <a href=\"" + action + "?format=html&timeout=3600000\">"
                "[ 1h ]"
              "</a>"
              " <a href=\"" + action + "?format=html&timeout=21600000\">"
                "[ 6h ]"
              "</a>"
              " <a href=\"" + action + "?format=html&timeout=43200000\">"
                "[ 12h ]"
              "</a>"
              " <a href=\"" + action + "?format=html&timeout=86400000\">"
                "[ 1d ]"
              "</a>"
              " <a href=\"" + action + "?format=html&timeout=604800000\">"
                "[ 1w ]"
              "</a>"
              " <a href=\"" + action + "?format=html&timeout=2628000000\">"
                "[ 1mo ]"
              "</a>";
    } else if (action.equals("/status")) {
      res +=
              action +
              " <a href=\"" + action + "?format=html\">" +
                "[ HTML ]"
              "</a>"
              " <a href=\"" + action + "?format=json\">" +
                "[ JSON ]"
              "</a>";
    } else {
      res +=
              "<a href=\"" + action + "?format=html\">" +
                action +
              "</a>";
    }
    res +=
          "</li>";
  }
  res +=
        "</ul>"
      "</body>"
    "</html>";

  return res;
}

void handleRoot() {

  printHttpRequest();
  server.send(200, "text/html", getHttpRootResponse());
}

void httpConnect() {

  if (!httpEnabled) {
    return;
  }

  server.begin();
  logSerialMessage("HTTP server started on port " + (String)httpServerPort);
}

void httpDisconnect() {

  server.close();
  logSerialMessage("HTTP server stopped");
}

void httpSetup() {

  logSerialMessage("\n--- HTTP setup ---");

  httpTitle = (String)rootName + "-" + commonName + "-" + uniqueId;

  server.on("/", HTTP_GET, handleRoot);
  server.on("/on", HTTP_GET, handleOn);
  server.on("/off", HTTP_GET, handleOff);
  server.on("/toggle", HTTP_GET, handleToggle);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/enable-noise", HTTP_GET, handleEnableNoise);
  server.on("/disable-noise", HTTP_GET, handleDisableNoise);
  server.on("/enable-timer", HTTP_GET, handleEnableTimer);
  server.on("/disable-timer", HTTP_GET, handleDisableTimer);
  server.on("/set-timer", HTTP_GET, handleSetTimer);
  server.on("/enable-mqtt", HTTP_GET, handleEnableMqtt);
  server.on("/disable-mqtt", HTTP_GET, handleDisableMqtt);
  server.on("/enable-ota", HTTP_GET, handleEnableOta);
  server.on("/disable-ota", HTTP_GET, handleDisableOta);
  server.on("/reset", HTTP_GET, handleReset);

  server.onNotFound(handleNotFound);

  httpConnect();
}

void evalHttpStatus(uint32_t currEvalTime) {

  if (!httpEnabled || !getWifiStatus()) {
    return;
  }

  if ((uint32_t)(currEvalTime - lastHttpEvalTime) < httpEvalTimeout) {
    return;
  }
  lastHttpEvalTime = currEvalTime;

  server.handleClient();
}

void enableHttp() {

  if (httpEnabled) {
    return;
  }

  httpEnabled = true;
  httpConnect();
  logMessage("HTTP communication enabled");
}

void disableHttp() {

  if (!httpEnabled) {
    return;
  }

  httpEnabled = false;
  httpDisconnect();
  logMessage("HTTP communication disabled");
}

bool getHttpEnabled() {

  return httpEnabled;
}
