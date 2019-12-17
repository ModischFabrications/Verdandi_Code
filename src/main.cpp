//#include <Arduino.h>
#include <ESP8266WiFi.h> // we have WiFi
#include <ESP8266HTTPClient.h>  // connect to API
#include <ESP8266WebServer.h>  // config portal
#include <WiFiManager.h> // captive portal for wifi

// define before importing
#define DEBUG
#include "serialWrapper.h"

// could (should?) be moved into other files
const uint8_t PIN_RGB = D1;
const uint8_t N_LEDS = 24;

WiFiManager wifiManager;
HTTPClient httpClient;

// gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager* myWiFiManager) {
    println(F("Entered config mode"));
    // auto generated SSID might be unknown
    printlnRaw(myWiFiManager->getConfigPortalSSID());
    printlnRaw(WiFi.softAPIP().toString());
}

void test_connection() {
    httpClient.begin(F("http://worldtimeapi.org/api/ip"));
    int httpCode = httpClient.GET();
    
    // String payload = http.getString();

    if (httpCode != HTTP_CODE_OK) {
        print(F("Connection test failed with status code "));
        printlnRaw(String(httpCode));
        return;
    }

    println(F("internet connection established"));
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

    // TODO: display connection error
    test_connection();

    // if you get here you have connected to the WiFi
    println(F("connected...yeey :)"));
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    // light LED whenever user interaction is needed
    // BUILTIN is inverted (LOW -> ON)
    digitalWrite(LED_BUILTIN, LOW);

    setup_serial(9600);
    setup_WiFi();

    // ...

    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    delay(100);

    heartbeat_serial();
}
