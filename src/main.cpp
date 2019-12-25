#include <Arduino.h>
#include <ESP8266HTTPClient.h> // connect to API

// define before importing
#define DEBUG
#include "SerialWrapper.h"

#include "persistence/persistenceManager.h"

#include "network/WiFiLoginManager.h"
#include "network/ConfigPortal.h"

// could (should?) be moved into other files
const uint8_t PIN_RGB = D1;
const uint8_t N_LEDS = 24;

HTTPClient http;
WiFiClient client;

void test_internet_connection() {
    http.begin(client, F("http://worldtimeapi.org/api/ip"));
    int httpCode = http.GET();

    if (httpCode > 0) {
        print(F("[HTTP] GET... code: "));
        printlnRaw((String)httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK ||
            httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = http.getString();
            printlnRaw(payload);
        }
    } else {
        printRaw("[HTTP] GET... failed, error: ");
        printlnRaw(http.errorToString(httpCode).c_str());
    }

    println(F("connected to the internet"));

    http.end();
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

    PersistenceManager::try_save();
    
    // TODO: remove or extract
    String received = readString();
    if (received == "L") {
        Configuration config = PersistenceManager::get();
        printlnRaw(String(config.brightness));
    }
    if (received == "S") {
        Configuration config = {0};
        PersistenceManager::set(config);
    }

    heartbeat_serial();
}
