#pragma once

#include <Arduino.h>

#ifdef DEBUG
const bool USE_SERIAL = true;
#else
const bool USE_SERIAL = false;
#endif

void setup_serial(int baud)
{
    if (!USE_SERIAL)
        return;

    Serial.begin(baud);
    Serial.println(); // init monitor
    Serial.println(F("Welcome to Verdandi"));
    Serial.println(F("Debug mode is activated"));
}

void heartbeat_serial()
{
    if (!USE_SERIAL)
        return;

    static uint32_t last_msg = 0;
    const uint16_t time_interval = 5 * 1000;

    uint32_t time = millis();

    if (time - last_msg > time_interval)
    {
        Serial.print(F("."));
        last_msg = time;
    }
}

// protected method to enforce "F" and ensure flag usage
void println(const __FlashStringHelper *string)
{
    if (!USE_SERIAL)
        return;
    Serial.println(string);
}

void printlnRaw(String string)
{
    if (!USE_SERIAL)
        return;
    Serial.println(string);
}

