#pragma once

#include <ESP8266WebServer.h> // config portal

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

    String json = F("{");
    json += F("\"brightness\":\"");
    json += config.brightness;
    json += F("\",\"show_hours\":\"");
    json += config.show_hours;
    json += F("\",\"show_minutes\":\"");
    json += config.show_minutes;
    json += F("\",\"show_seconds\":\"");
    json += config.show_seconds;
    json += F("\",\"colorH\":[");
    json += config.colorH[0];
    json += F(",");
    json += config.colorH[1];
    json += F(",");
    json += config.colorH[2];
    json += F("],\"colorM\":[");
    json += config.colorM[0];
    json += F(",");
    json += config.colorM[1];
    json += F(",");
    json += config.colorM[2];
    json += F("],\"colorS\":[");
    json += config.colorS[0];
    json += F(",");
    json += config.colorS[1];
    json += F(",");
    json += config.colorS[2];
    json += F("],\"poll_interval\":");
    json += config.poll_interval_min;
    json += F("}");
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