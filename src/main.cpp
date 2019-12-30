#include <Arduino.h>
#include <ESP8266HTTPClient.h> // connect to API

// define before importing anything
#define DEBUG

#include "SerialWrapper.h"
#include "serialInputTester.h"

#include "persistence/persistenceManager.h"

#include "network/ConfigPortal.h"
#include "network/WiFiLoginManager.h"
#include "network/timeService.h"

#ifdef DEBUG
const bool DEBUG_MODE = true;
#else
const bool DEBUG_MODE = false;
#endif

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
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
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
    PersistenceStore::setup();
    WiFiLoginManager::setup();
    ConfigPortal::setup_config_portal();
    TimeService::setup();

    // TODO: display connection error
    test_internet_connection();

    // ...

    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    // reduce as needed
    delay(10);

    if (DEBUG_MODE) {
        SerialInputTest::handleInput();
    }

    ConfigPortal::check();

    PersistenceManager::try_save();

    heartbeat_serial();
}
