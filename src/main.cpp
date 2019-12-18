//#include <Arduino.h>
#include <ESP8266HTTPClient.h> // connect to API
#include <ESP8266WebServer.h>  // config portal
#include <ESP8266WiFi.h>       // we have WiFi
#include <WiFiManager.h>       // captive portal for wifi

// define before importing
#define DEBUG
#include "SerialWrapper.h"
#include "Website.h"

// could (should?) be moved into other files
const uint8_t PIN_RGB = D1;
const uint8_t N_LEDS = 24;

WiFiManager wifiManager;
HTTPClient httpClient;
ESP8266WebServer server(80);

// gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager* myWiFiManager) {
    println(F("Entered config mode"));
    // auto generated SSID might be unknown
    printlnRaw(myWiFiManager->getConfigPortalSSID());
    printlnRaw(WiFi.softAPIP().toString());
}

void test_internet_connection() {
    httpClient.begin(F("http://worldtimeapi.org/api/ip"));
    int httpCode = httpClient.GET();
    String payload = httpClient.getString();
    printlnRaw(payload);
    httpClient.end();

    if (httpCode != HTTP_CODE_OK) {
        print(F("Connection test failed with status code "));
        printlnRaw(String(httpCode));
        return;
    }

    println(F("connected to the internet"));
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

void handle_data_update() {
    // send back information about arguments as a test
    String message = "Number of args received:";
    message += server.args();

    for (int i = 0; i < server.args(); i++) {
        message += "Arg nº" + (String)i + " –> ";
        message += server.argName(i) + ": ";
        message += server.arg(i) + "\n";
    }

    server.send(200, "text/plain", message);
}

void handle_server() { server.send(200, "text/html", getContent()); }

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    // light LED whenever user interaction is needed
    // BUILTIN is inverted (LOW -> ON)
    digitalWrite(LED_BUILTIN, LOW);

    setup_serial(115200);
    setup_WiFi();

    // TODO: display connection error
    test_internet_connection();

    // ...

    server.on("/", handle_server);
    server.on("/update", handle_data_update);
    server.begin();
    println(F("Server started"));

    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    delay(100);

    server.handleClient();

    heartbeat_serial();
}
