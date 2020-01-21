#pragma once

#include <ArduinoJson.h>
#include <ESP8266WebServer.h> // config portal
#include <string.h>

#include "fileServer.h"
#include "persistence/persistenceManager.h"
#include "serial/SerialWrapper.h"

namespace ConfigPortal {

void setup();
void handleServer();
void handleConfigRequest();
void handleDataUpdate();
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
    server.onNotFound(handleServer);
    server.begin();
    println(F("Server started"));
}

void handleServer() {
    String path = server.uri();
    if (path.endsWith("/"))
        path += "index.html";

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

void handleConfigRequest() {
    println(F("Received config request"));
    Config::Configuration config = PersistenceManager::get();

    // actual number of objects, in this case number of lines
    const uint16_t capacity = JSON_OBJECT_SIZE(26);
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
    doc["timezone_name"] = (const char*)config.timezone_name;
    // 26 elements

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

void splitCSVToArray(const char* input, uint8_t* output) {
    // unsafe and shit code, but it could work
    char buffer[16];
    strcpy(buffer, input);
    char* segment_holder;
    segment_holder = strtok(buffer, ",");

    uint8_t i = 0;
    while (segment_holder != NULL) {
        printlnRaw(segment_holder);
        segment_holder = strtok(NULL, ",");
        if (segment_holder == NULL)
            continue;
        char* unused_tail;
        uint8_t color = strtol(segment_holder, &unused_tail, 10);
        // really unsafe usage here!
        output[i++] = color;
    }
}

Config::Configuration ArgsToConfiguration() {
    Config::Configuration newValues = Config::Configuration();

    // implicit clamping could make problems but it's just nicer to read
    if (server.argName(0) == "brightness")
        newValues.brightness = server.arg(0).toInt();
    else
        println(F("Value 0 not found"));

    if (server.argName(1) == "showHours")
        newValues.showHours = server.arg(1) == "true" ? true : false;
    else
        println(F("Value 1 not found"));
    if (server.argName(2) == "showMinutes")
        newValues.showMinutes = server.arg(2) == "true" ? true : false;
    else
        println(F("Value 2 not found"));
    if (server.argName(3) == "showSeconds")
        newValues.showSeconds = server.arg(3) == "true" ? true : false;
    else
        println(F("Value 3 not found"));

    if (server.argName(4) == "colorH") {
        const char* input = server.arg(4).c_str();
        splitCSVToArray(input, newValues.colorH);
    } else
        println(F("Value 4 not found"));

    if (server.argName(4) == "colorM") {
        const char* input = server.arg(5).c_str();
        splitCSVToArray(input, newValues.colorM);
    } else
        println(F("Value 5 not found"));

    if (server.argName(6) == "colorH") {
        const char* input = server.arg(6).c_str();
        splitCSVToArray(input, newValues.colorS);
    } else
        println(F("Value 6 not found"));

    if (server.argName(7) == "pollInterval")
        newValues.pollInterval = max(server.arg(7).toInt(), (long)1);
    else
        println(F("Value 7 not found"));

    // TODO: add fields for nightmode and Times

    if (server.argName(8) == "nightmode")
        newValues.nightmode = server.arg(8) == "true" ? true : false;
    else
        println(F("Value 8 not found"));
    if (server.argName(9) == "turnOffAt") {
        newValues.turnOffAt = Time{(uint8_t)server.arg(9).toInt()};
        const char* input = server.arg(4).c_str();
        uint8_t times[2];
        splitCSVToArray(input, times);
        newValues.turnOffAt = Time{times[0], times[1]};
    } else
        println(F("Value 9 not found"));
    if (server.argName(10) == "turnOnAt") {
        newValues.turnOnAt = Time{(uint8_t)server.arg(9).toInt()};
        const char* input = server.arg(4).c_str();
        uint8_t times[2];
        splitCSVToArray(input, times);
        newValues.turnOnAt = Time{times[0], times[1]};
    } else
        println(F("Value 10 not found"));

    if (server.argName(11) == "timezone")
        strcpy(newValues.timezone, server.arg(11).c_str());
    else
        println(F("Value 11 not found"));
    if (server.argName(12) == "timezone_name")
        strcpy(newValues.timezone_name, server.arg(12).c_str());
    else
        println(F("Value 12 not found"));

    // add new fields here

    return newValues;
}

} // namespace ConfigPortal