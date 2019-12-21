#pragma once

#include <ESP8266WebServer.h> // config portal

#include "SerialWrapper.h"
#include "Website.h"

ESP8266WebServer server(80);

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

void handle_server() { server.send(200, "text/html", getContent()); }
