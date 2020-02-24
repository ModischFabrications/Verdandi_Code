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

    if (!MDNS.begin("verdandi")) { // Start the mDNS responder
        logWarning(F("Error setting up mDNS responder!"));
    }

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

    heartbeatSerial();
}
