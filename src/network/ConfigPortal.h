#pragma once

#include <ESP8266WebServer.h> // config portal
#include <ArduinoJson.h>

#include "network/Website.h"
#include "persistence/persistenceManager.h"
#include "serial/SerialWrapper.h"

namespace ConfigPortal {

void setup_config_portal();
void handle_server();
void handle_config_request();
void handle_data_update();
void check();

namespace {
ESP8266WebServer server(80);
}

void check() {
    server.handleClient();

    // put everything with regular updates in here
}

void setup_config_portal() {
    server.on(F("/"), handle_server);
    server.on(F("/config"), handle_config_request);
    server.on(F("/update"), handle_data_update);
    server.begin();
    println(F("Server started"));
}

void handle_server() { server.send(200, "text/html", Website::getContent()); }

void handle_config_request() {
    println(F("Received config request"));
    Configuration config = PersistenceManager::get();

    const uint16_t capacity = JSON_OBJECT_SIZE(17);
    StaticJsonDocument<capacity> doc;

    doc["brightness"] = config.brightness;
    doc["showHours"] = config.show_hours;
    doc["showMinutes"] = config.show_minutes;
    doc["showSeconds"] = config.show_seconds;
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
    doc["pollInterval"] = config.poll_interval_min;

    String json = "";
    serializeJson(doc, json);
    printlnRaw(json);

    server.send(200, F("application/json"), json);
}

void handle_data_update() {
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