#pragma once

// #include <NTPClient.h>
#include <WiFiUdp.h>
#include <coredecls.h>
#include <sntp.h>
#include <sys/time.h>
#include <time.h>

#include "TZ.h"
#include "Time.h"
#include "persistence/persistenceManager.h"
#include "serial/SerialWrapper.h"

uint32_t sntp_update_delay_MS_rfc_not_less_than_15000() {
    uint16_t interval = max(PersistenceManager::get().pollInterval, (uint16_t)1);
    return interval * 60 * 1000;
}

namespace TimeService {

bool initializedOffset = false;

void setup();
void timeUpdate();
Time getCurrentTime();

namespace {

timeval tv;
Time currentTime = {0, 0, 0, 0};
uint16_t millisOffset = 0;

void updateConfiguration();
void initializeMillisOffset();

// ---------

void initializeMillisOffset() {
    gettimeofday(&tv, nullptr);
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    uint8_t initSecond = t->tm_sec;
    uint8_t nextSecond = initSecond;

    while (initSecond == nextSecond) {
        // please don't kill me watchdog
        delay(0);
        gettimeofday(&tv, nullptr);
        now = time(nullptr);
        t = localtime(&now);
        nextSecond = t->tm_sec;
    }

    // our measurement is never exactly precise!
    // we archieve +- 10ms
    millisOffset = 1000 - (millis() % 1000);

    println(F("Synchronized with NTP server"));
    initializedOffset = true;
}

void updateConfiguration() {
    Config::Configuration config = PersistenceManager::get();
    configTime(config.timezone, "pool.ntp.org");

    print(F("Setting new timezone to "));
    printlnRaw(config.timezone);
    // pollInterval fetched live, no need to cache
}

} // namespace

void setup() {
    settimeofday_cb(initializeMillisOffset);
    PersistenceManager::registerListener(updateConfiguration);

    // updateConfiguration will be called automatically when registering
}

void timeUpdate() {
    if (!initializedOffset) {
        return;
    }
    Time newTime = currentTime;

    // TODO: check how it get the new time
    gettimeofday(&tv, nullptr);
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    newTime.hour = t->tm_hour;
    newTime.minute = t->tm_min;
    newTime.second = t->tm_sec;
    newTime.millisecond = (millis() + millisOffset) % 1000;

    if (newTime.hour >= 24) logError(F("hour overflow"));
    if (newTime.minute >= 60) logError(F("minute overflow"));
    if (newTime.second >= 60) logError(F("second overflow"));
    if (newTime.millisecond >= 1000) logError(F("millisecond overflow"));

    if (newTime.second == currentTime.second && newTime.millisecond < currentTime.millisecond) {
        // ~ every 5sec, we don't care that bad
        // logError(F("Non continuous time update, ignoring"));
        return;
    }

    currentTime = newTime;
}

Time getCurrentTime() { return currentTime; }
} // namespace TimeService
