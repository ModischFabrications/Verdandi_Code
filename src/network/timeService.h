#pragma once

// #include <NTPClient.h>
#include <WiFiUdp.h>
#include <sntp.h>
#include <sys/time.h>
#include <time.h>
#include <coredecls.h>

#include "TZ.h"
#include "serial/SerialWrapper.h"
#include "persistence/persistenceManager.h"
#include "Time.h"

uint32_t sntp_update_delay_MS_rfc_not_less_than_15000 ()
{
   return PersistenceManager::get().pollInterval * 60 * 1000;
}

namespace TimeService {

bool initializedOffset = false;

void setup();
void timeUpdate();
Time getCurrentTime();

namespace {
const char* TIMEZONE = "TZ_Europe_London";

timeval tv;
Time currentTime = {0, 0, 0, 0};
uint16_t millisOffset = 0;

void initializeMillisOffset();

void initializeMillisOffset() {
    gettimeofday(&tv, nullptr);
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    uint8_t initSecond = t->tm_sec;
    uint8_t nextSecond = initSecond;

    while (initSecond == nextSecond) {
        gettimeofday(&tv, nullptr);
        now = time(nullptr);
        t = localtime(&now);
        nextSecond = t->tm_sec;
        delay(1);
    }
    millisOffset = 1000 - (millis() % 1000);

    println(F("Synchronized with NTP server."));
    initializedOffset = true;
}
} // namespace

void setup() {
    configTime(TIMEZONE, "pool.ntp.org");
    // TODO: add listener to change timezone and poll interval

    settimeofday_cb(initializeMillisOffset);
}

void timeUpdate() {
    if (!initializedOffset) {
        return;
    }
    // TODO: check how it get the new time
    gettimeofday(&tv, nullptr);
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    currentTime.hour = t->tm_hour;
    currentTime.minute = t->tm_min;
    currentTime.second = t->tm_sec;
    currentTime.millisecond = (millis() + millisOffset) % 1000;
}

Time getCurrentTime() { return currentTime; }
} // namespace TimeService
