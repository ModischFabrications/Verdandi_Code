#pragma once

#include <ESP8266WebServer.h> // config portal
#include <ArduinoJson.h>

#include "network/Website.h"
#include "persistence/persistenceManager.h"
#include "serial/SerialWrapper.h"

namespace ConfigPortal {

void setup();
void handleServer();
void handleConfigRequest();
void handleDataUpdate();
void check();

namespace {
ESP8266WebServer server(80);
}

void check() {
    server.handleClient();

    // put everything with regular updates in here
}

void setup() {
    server.on(F("/"), handleServer);
    server.on(F("/config"), handleConfigRequest);
    server.on(F("/update"), handleDataUpdate);
    server.begin();
    println(F("Server started"));
}

void handleServer() { server.send(200, "text/html", Website::getContent()); }

void handleConfigRequest() {
    println(F("Received config request"));
    Configuration config = PersistenceManager::get();

    const uint16_t capacity = JSON_OBJECT_SIZE(17);
    StaticJsonDocument<capacity> doc;

    doc["brightness"] = config.brightness;
    doc["showHours"] = config.showHours;
    doc["showMinutes"] = config.showMinutes;
    doc["showSeconds"] = config.showSeconds;
    JsonArray colorH = doc.createNestedArray("colorH");
    JsonArray colorM = doc.createNestedArray("colorM");
    JsonArray colorS = doc.createNestedArray("colorS");
    colorH.add(config.colorH[0]);
    colorH.add(config.colorH[1]);
    colorH.add(config.colorH[2]);
    colorM.add(config.colorM[0]);
    colorM.add(config.colorM[1]);
    colorM.add(config.colorM[2]);
    colorS.add(config.colorS[0]);
    colorS.add(config.colorS[1]);
    colorS.add(config.colorS[2]);
    doc["pollInterval"] = config.pollInterval;

    String json = "";
    serializeJson(doc, json);
    printlnRaw(json);

    server.send(200, F("application/json"), json);
}

void handleDataUpdate() {
    // send back information about arguments as a test
    String message = "Number of args received: ";
    message += server.args();

    for (int i = 0; i < server.args(); i++) {
        message += "\nArg #" + (String)i + " >> ";
        message += server.argName(i) + ": ";
        message += server.arg(i);
    }

    printlnRaw(message);
    server.send(200, "text/plain", message);
}
} // namespace ConfigPortal