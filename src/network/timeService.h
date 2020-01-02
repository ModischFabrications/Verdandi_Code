#pragma once

// #include <NTPClient.h>
#include <WiFiUdp.h>
#include <sntp.h>
#include <sys/time.h>
#include <time.h>

#include "TZ.h"
#include "serial/SerialWrapper.h"

namespace TimeService {

struct Time {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
};

namespace {
const char* TIMEZONE = "TZ_Europe_London";

timeval tv;
Time currentTime = {0, 0, 0, 0};
} // namespace

void setup();
void timeUpdate();
Time getCurrentTime();

void setup() {
    configTime(TIMEZONE, "pool.ntp.org");
    // TODO: add listener to change timezone and poll interval
}

void timeUpdate() {
    // TODO: check how it get the new time
    gettimeofday(&tv, nullptr);
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    currentTime.hour = t->tm_hour;
    currentTime.minute = t->tm_min;
    currentTime.second = t->tm_sec;
    currentTime.millisecond = millis() % 1000;
}

Time getCurrentTime() { return currentTime; }
} // namespace TimeService
