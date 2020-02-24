#pragma once

#include <ArduinoJson.h>
#include <ESP8266WebServer.h> // config portal
#include <string.h>

#include "Time.h"
#include "fileServer.h"
#include "network/timeService.h"
#include "persistence/persistenceManager.h"
#include "serial/SerialWrapper.h"

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
String ArgsToString();
Config::Configuration ArgsToConfiguration();

namespace {
ESP8266WebServer server(80);
}

void check() {
    server.handleClient();

    // put everything with regular updates in here
}

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

void handleServer() {
    String path = server.uri();
    if (path.endsWith("/"))
        path += "index.html";
    handleFile(path);
}

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

void handleDebug() { handleFile("/debug.html"); }

void handleLogRequest() {
    const RingBuffer& warnings = getWarnLog();
    const RingBuffer& errors = getErrorLog();

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

void handleConfigRequest() {
    println(F("Received config request"));
    Config::Configuration config = PersistenceManager::get();

    // actual number of objects, in this case number of lines
    const uint16_t capacity = JSON_OBJECT_SIZE(27);
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
    // 17 elements up to here

    doc["pollInterval"] = config.pollInterval;
    doc["nightmode"] = config.nightmode;

    JsonArray turnOffAt = doc.createNestedArray("turnOffAt");
    JsonArray turnOnAt = doc.createNestedArray("turnOnAt");
    turnOffAt.add(config.turnOffAt.hour);
    turnOffAt.add(config.turnOffAt.minute);
    turnOnAt.add(config.turnOnAt.hour);
    turnOnAt.add(config.turnOnAt.minute);

    doc["timezone"] = (const char*)config.timezone;
    doc["timezoneName"] = (const char*)config.timezoneName;

    doc["useFading"] = config.useFading;
    // 27 elements

    // add new fields here and increase JSON size

    String json = "";
    serializeJson(doc, json);
    printlnRaw(json);

    server.send(200, F("application/json"), json);
}

void handleDataUpdate() {
    // send back information about arguments as a test

    printlnRaw(ArgsToString());
    PersistenceManager::set(ArgsToConfiguration());

    // everything is fine
    server.send(200);
}

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

String ArgsToString() {
    String message = "Number of args received: ";
    message += server.args();

    for (int i = 0; i < server.args(); i++) {
        message += "\nArg #" + (String)i + " >> ";
        message += server.argName(i) + ": ";
        message += server.arg(i);
    }

    return message;
}

void splitCSVToArray(const char* input, uint8_t* output, const char* token = ",") {
    // unsafe and shit code, but it could work
    char buffer[16];
    strcpy(buffer, input);
    char* segment_holder;
    segment_holder = strtok(buffer, token);

    uint8_t i = 0;
    while (segment_holder != NULL) {
        char* unused_tail;
        uint8_t color = strtol(segment_holder, &unused_tail, 10);
        // really unsafe usage here!
        output[i++] = color;

        segment_holder = strtok(NULL, token);
    }
}

Config::Configuration ArgsToConfiguration() {
    Config::Configuration newValues = Config::Configuration();

    // implicit clamping could make problems but it's just nicer to read
    if (server.argName(0) == "brightness")
        newValues.brightness = server.arg(0).toInt();
    else
        logError(F("Value 0 not found"));

    if (server.argName(1) == "showHours")
        newValues.showHours = server.arg(1) == "true" ? true : false;
    else
        logError(F("Value 1 not found"));
    if (server.argName(2) == "showMinutes")
        newValues.showMinutes = server.arg(2) == "true" ? true : false;
    else
        logError(F("Value 2 not found"));
    if (server.argName(3) == "showSeconds")
        newValues.showSeconds = server.arg(3) == "true" ? true : false;
    else
        logError(F("Value 3 not found"));

    if (server.argName(4) == "colorH") {
        const char* input = server.arg(4).c_str();
        splitCSVToArray(input, newValues.colorH);
        printArray(newValues.colorH, 3);
    } else
        logError(F("Value 4 not found"));

    if (server.argName(5) == "colorM") {
        const char* input = server.arg(5).c_str();
        splitCSVToArray(input, newValues.colorM);
        printArray(newValues.colorM, 3);
    } else
        logError(F("Value 5 not found"));

    if (server.argName(6) == "colorS") {
        const char* input = server.arg(6).c_str();
        splitCSVToArray(input, newValues.colorS);
        printArray(newValues.colorS, 3);
    } else
        logError(F("Value 6 not found"));

    if (server.argName(7) == "pollInterval")
        newValues.pollInterval = max(server.arg(7).toInt(), (long)1);
    else
        logError(F("Value 7 not found"));

    if (server.argName(8) == "nightmode")
        newValues.nightmode = server.arg(8) == "true" ? true : false;
    else
        logError(F("Value 8 not found"));
    if (server.argName(9) == "turnOffAt") {
        newValues.turnOffAt = Time{(uint8_t)server.arg(9).toInt()};
        const char* input = server.arg(9).c_str();
        uint8_t times[2];

        splitCSVToArray(input, times, ":");
        newValues.turnOffAt = Time{times[0], times[1]};
    } else
        logError(F("Value 9 not found"));
    if (server.argName(10) == "turnOnAt") {
        newValues.turnOnAt = Time{(uint8_t)server.arg(10).toInt()};
        const char* input = server.arg(10).c_str();
        uint8_t times[2];
        splitCSVToArray(input, times, ":");
        newValues.turnOnAt = Time{times[0], times[1]};
    } else
        logError(F("Value 10 not found"));

    if (server.argName(11) == "timezone")
        strcpy(newValues.timezone, server.arg(11).c_str());
    else
        logError(F("Value 11 not found"));
    if (server.argName(12) == "timezoneName")
        strcpy(newValues.timezoneName, server.arg(12).c_str());
    else
        logError(F("Value 12 not found"));
    if (server.argName(13) == "useFading")
        newValues.useFading = server.arg(13) == "true" ? true : false;
    else
        logError(F("Value 13 not found"));


    // add new fields here

    return newValues;
}

} // namespace ConfigPortal