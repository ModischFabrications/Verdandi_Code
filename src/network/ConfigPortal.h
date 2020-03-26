#pragma once

#include <ArduinoJson.h>
#include <ESP8266WebServer.h> // config portal
#include <string.h>

#include "Time.h"
#include "fileServer.h"
#include "network/timeService.h"
#include "persistence/configuration.h"
#include "persistence/persistenceManager.h"
#include "serial/SerialWrapper.h"
#include "network/RequestParser.h"

namespace ConfigPortal {

void setup();
void handleServer();
void handleFile(String path);
void handleConfigRequest();
void handleDataUpdate();
void handleTimeRequest();
void handleDebug();
void handleLogRequest();
void check();
// String ArgsToString();
// Config::Configuration ArgsToConfiguration();

namespace {
ESP8266WebServer server(80);
}

void check() {
    server.handleClient();

    // put everything with regular updates in here
}

// set up endpoints
void setup() {
    server.on(F("/config"), handleConfigRequest);
    server.on(F("/update"), handleDataUpdate);
    server.on(F("/time"), handleTimeRequest);
    server.on(F("/debug"), handleDebug);
    server.on(F("/debug/messages"), handleLogRequest);
    server.onNotFound(handleServer);
    server.begin();
    println(F("Server started"));
}

// process undefined endpoints or endpoints to websites
void handleServer() {
    // look for a html file within the path of the server
    String path = server.uri();
    if (path.endsWith("/"))
        path += "index.html";
    handleFile(path);
}

/**
 * send requested file
 * @param path path to the file in the data folder
 * */
void handleFile(String path) {
    if (FileServer::fileExists(path)) {
        File f = FileServer::getFile(path);
        String contentType = FileServer::getContentType(path);
        server.streamFile(f, contentType);
        f.close();
    } else {
        print(F("File '"));
        printRaw(path);
        println(F("' does not exist"));

        server.send(404, "text/html", "Requested file not found. Reupload the file system image.");
    }
}

// send debug file
void handleDebug() { handleFile("/debug.html"); }

// process request for all log and error messages
void handleLogRequest() {
    const RingBuffer& warnings = getWarnLog();
    const RingBuffer& errors = getErrorLog();

    // gather warnings and errors and create a json string from them
    // TODO: seems to be wrong, capped to N_LOGS - 5
    const uint16_t capacity = JSON_OBJECT_SIZE(2 * N_MAX_LOGS + 4);
    StaticJsonDocument<capacity> doc;

    doc["warningIndex"] = warnings.iLog;
    doc["errorIndex"] = errors.iLog;
    JsonArray warningsField = doc.createNestedArray("warnings");
    JsonArray errorsField = doc.createNestedArray("errors");

    for (uint8_t i = 0; i < N_MAX_LOGS; ++i) {
        if (warnings.log[i] != nullptr) {
            warningsField.add(warnings.log[i]);
        }
        if (errors.log[i] != nullptr) {
            errorsField.add(errors.log[i]);
        }
    }
    String json = "";
    serializeJson(doc, json);
    printlnRaw(json);

    server.send(200, F("application/json"), json);
}

// process request to the lates config
void handleConfigRequest() {
    println(F("Received config request"));
    String json = RequestParser::generateConfigJson();;

    server.send(200, F("application/json"), json);
}

// process server receiving a new configuration
void handleDataUpdate() {
    printlnRaw(RequestParser::argsToString(server));
    PersistenceManager::set(RequestParser::argsToConfiguration(server));

    // everything is fine
    server.send(200);
}

// send the current time
void handleTimeRequest() {
    println(F("Received time request"));

    Time t = TimeService::getCurrentTime();

    // actual number of objects, in this case number of lines
    const uint16_t capacity = JSON_OBJECT_SIZE(3);
    StaticJsonDocument<capacity> doc;

    doc["hours"] = t.hour;
    doc["minutes"] = t.minute;
    doc["seconds"] = t.second;

    String json = "";
    serializeJson(doc, json);
    printlnRaw(json);

    server.send(200, F("application/json"), json);
}
} // namespace ConfigPortal