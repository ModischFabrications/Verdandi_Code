#include <Arduino.h>
#include <ESP8266HTTPClient.h> // connect to API
#include <ESP8266mDNS.h>

// define before importing anything
#define DEBUG

#include "serial/SerialWrapper.h"
#include "serial/serialInputTester.h"

#include "persistence/persistenceManager.h"

#include "network/ConfigPortal.h"
#include "network/WiFiLoginManager.h"
#include "network/timeService.h"

#include "led/ledController.h"

#include "fileServer.h"

#include "led/ledController.h"
#include <FastLED.h>

#ifdef DEBUG
const bool DEBUG_MODE = true;
#else
const bool DEBUG_MODE = false;
#endif

WiFiClient client;

void testInternetConnection() {
    HTTPClient http;

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
        logError(F("No connection to the internet"));
    }

    println(F("connected to the internet"));

    http.end();
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    // light LED whenever user interaction is needed
    // BUILTIN is inverted (LOW -> ON)
    digitalWrite(LED_BUILTIN, LOW);

    setupSerial(115200);
    PersistenceStore::setup();
    FileServer::setup();
    // reset watchdog
    delay(0);
    WiFiLoginManager::setup();
    // reset watchdog
    delay(0);
    TimeService::setup();
    LedController::setup();
    ConfigPortal::setup();

    if (!MDNS.begin("verdandi")) { // Start the mDNS responder for esp8266.local
        logWarning(F("Error setting up mDNS responder!"));
    }

    // TODO: display connection error
    testInternetConnection();

    // ...

    // reset watchdog
    delay(0);
    digitalWrite(LED_BUILTIN, HIGH);

    LedController::helloPower();
}

void loop() {
    // reduce as needed
    FastLED.delay(1);

    if (DEBUG_MODE) {
        SerialInputTest::handleInput();
    }

    TimeService::timeUpdate();

    ConfigPortal::check();

    PersistenceManager::trySave();

    LedController::tick();

    // heartbeatSerial();
}
