#include <Arduino.h>
#include <ESP8266HTTPClient.h> // connect to API

// define before importing anything
#define DEBUG

#include "serial/SerialWrapper.h"
#include "serial/serialInputTester.h"

#include "persistence/persistenceManager.h"

#include "network/ConfigPortal.h"
#include "network/WiFiLoginManager.h"
#include "network/timeService.h"

#include "fileServer.h"

#include "led/ledController.h"
#include <FastLED.h>

#ifdef DEBUG
const bool DEBUG_MODE = true;
#else
const bool DEBUG_MODE = false;
#endif

HTTPClient http;
WiFiClient client;

// TODO: remove as soon as ntp time fetch works
void testInternetConnection() {
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

void testListener() {
    println(F("Test listener called"));
    logWarning(F("Test Warning"));
    logError(F("Test Error"));

    RingBuffer warnings = getWarnLog();
    RingBuffer errors = getErrorLog();

    println(warnings.log[warnings.iLog - 1]);
    println(errors.log[errors.iLog - 1]);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    // light LED whenever user interaction is needed
    // BUILTIN is inverted (LOW -> ON)
    digitalWrite(LED_BUILTIN, LOW);

    setupSerial(115200);
    PersistenceStore::setup();
    WiFiLoginManager::setup();
    ConfigPortal::setup();
    TimeService::setup();
    LedController::setup();
    FileServer::setup();

    // TODO: remove
    PersistenceManager::registerListener(testListener);
    PersistenceManager::registerListener(testListener);
    PersistenceManager::registerListener(testListener);

    // TODO: display connection error
    testInternetConnection();

    // ...

    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    // reduce as needed
    FastLED.delay(1);

    if (DEBUG_MODE) {
        SerialInputTest::handleInput();
    }

    ConfigPortal::check();

    PersistenceManager::trySave();

    heartbeatSerial();
}
