#pragma once

#include <NTPClient.h>
#include <WiFiUdp.h>

// TODO: make parameter
// utc offset for your timezone: e.g. GMT +1 = 3600
// can be set with timeClient.setTimeOffset(offset);
const long utcOffsetInSeconds = 3600;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup_ntp() {
    timeClient.begin();
}

void timeUpdate() {
    while(!timeClient.update()) {
        timeClient.forceUpdate();
    }

    String formattedTime = timeClient.getFormattedTime();
    print(F("TIME: "));
    printlnRaw(formattedTime);
}