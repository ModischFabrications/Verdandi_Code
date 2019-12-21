#pragma once

#include <WiFiManager.h> // captive portal for wifi

#include "SerialWrapper.h"

WiFiManager wifiManager;

// gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager* myWiFiManager) {
    println(F("Entered config mode"));
    // auto generated SSID might be unknown
    printlnRaw(myWiFiManager->getConfigPortalSSID());
    printlnRaw(WiFi.softAPIP().toString());
}

void setup_WiFi() {
    println(F("setting up wifi (with captive portal)"));

    // DEBUGGING:
    // wifiManager.resetSettings();

    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 1, 1),
                                    IPAddress(192, 168, 1, 1),
                                    IPAddress(255, 255, 255, 0));
    // wifiManager.setConfigPortalTimeout(60);

    if (!wifiManager.autoConnect("Verdandi")) {
        println(F("failed to connect and hit timeout"));
        // reset and try again after waking up
        // TODO: put it into deep sleep? How do we handle "broken"?
        ESP.restart();
        delay(1000);
        println(F("This should never happen..."));
    }

    // if you get here you have connected to the WiFi
    println(F("connected to the local network"));
}
