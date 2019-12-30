#pragma once

#include <NTPClient.h>
#include <WiFiUdp.h>

#include "SerialWrapper.h"

namespace TimeService {
namespace {
// TODO: make parameter
// utc offset for your timezone: e.g. GMT +1 = 3600
// can be set with timeClient.setTimeOffset(offset);
const long utcOffsetInSeconds = 3600;
} // namespace

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() { timeClient.begin(); }

void timeUpdate() {
    while (!timeClient.update()) {
        timeClient.forceUpdate();
    }

    String formattedTime = timeClient.getFormattedTime();
    print(F("TIME: "));
    printlnRaw(formattedTime);
}
} // namespace TimeService
