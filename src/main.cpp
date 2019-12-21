#include <Arduino.h>
#include <ESP8266HTTPClient.h> // connect to API

// define before importing
#define DEBUG
#include "SerialWrapper.h"
#include "WiFiLoginManager.h"

#include "ConfigPortal.h"

// could (should?) be moved into other files
const uint8_t PIN_RGB = D1;
const uint8_t N_LEDS = 24;

HTTPClient httpClient;

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
