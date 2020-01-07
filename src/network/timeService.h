#pragma once

// #include <NTPClient.h>
#include <WiFiUdp.h>
#include <sntp.h>
#include <sys/time.h>
#include <time.h>

#include "TZ.h"
#include "serial/SerialWrapper.h"
#include <coredecls.h>

namespace TimeService {

struct Time {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
};

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
