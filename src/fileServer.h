#pragma once

#include <Arduino.h>

#include "FS.h"
#include "serial/SerialWrapper.h"

namespace FileServer {
void setup();
String getContentType(String filename);
bool fileExists(String path);

void setup() { SPIFFS.begin(); }

String getContentType(String filename) {
    if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    else if (filename.endsWith(".gz"))
        return "application/x-gzip";
    return "text/plain";
}

bool fileExists(String path) {
    String pathWithGz = path + ".gz";
    return (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path));
}

File getFile(String path) {
    String pathWithGz = path + ".gz";
    if (SPIFFS.exists(pathWithGz))
        path += ".gz";
    print(F("Getting file from path: "));
    printlnRaw(path);
    return SPIFFS.open(path, "r");
}

} // namespace FileServer
