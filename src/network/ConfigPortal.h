#pragma once

#include <ArduinoJson.h>
#include <ESP8266WebServer.h> // config portal

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
    if (path.endsWith("/")) path += "index.html";

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
    // TODO: add new fields and increase JSON size

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

Config::Configuration ArgsToConfiguration() {
    Config::Configuration new_values = Config::Configuration();

    // implicit clamping could make problems but it's just nicer to read
    if (server.argName(0) == "brightness")
        new_values.brightness = server.arg(0).toInt();
    else
        println(F("Value 0 not found"));
    if (server.argName(1) == "showHours")
        new_values.showHours = server.arg(1) == "true" ? true : false;
    else
        println(F("Value 1 not found"));
    if (server.argName(2) == "showMinutes")
        new_values.showMinutes = server.arg(2) == "true" ? true : false;
    else
        println(F("Value 2 not found"));
    if (server.argName(3) == "showSeconds")
        new_values.showSeconds = server.arg(3) == "true" ? true : false;
    else
        println(F("Value 3 not found"));
    if (server.argName(4) == "colorH_R")
        new_values.colorH[0] = server.arg(4).toInt();
    else
        println(F("Value 4 not found"));
    if (server.argName(5) == "colorH_G")
        new_values.colorH[1] = server.arg(5).toInt();
    else
        println(F("Value 5 not found"));
    if (server.argName(6) == "colorH_B")
        new_values.colorH[2] = server.arg(6).toInt();
    else
        println(F("Value 6 not found"));
    if (server.argName(7) == "colorM_R")
        new_values.colorM[0] = server.arg(7).toInt();
    else
        println(F("Value 7 not found"));
    if (server.argName(8) == "colorM_G")
        new_values.colorM[1] = server.arg(8).toInt();
    else
        println(F("Value 8 not found"));
    if (server.argName(9) == "colorM_B")
        new_values.colorM[2] = server.arg(9).toInt();
    else
        println(F("Value 9 not found"));
    if (server.argName(10) == "colorS_R")
        new_values.colorS[0] = server.arg(10).toInt();
    else
        println(F("Value 10 not found"));
    if (server.argName(11) == "colorS_G")
        new_values.colorS[1] = server.arg(11).toInt();
    else
        println(F("Value 11 not found"));
    if (server.argName(12) == "colorS_B")
        new_values.colorS[2] = server.arg(12).toInt();
    else
        println(F("Value 12 not found"));
    if (server.argName(13) == "pollInterval")
        new_values.pollInterval = server.arg(13).toInt();
    else
        println(F("Value 13 not found"));

    return new_values;
}

} // namespace ConfigPortal