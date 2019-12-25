#pragma once

#include <Arduino.h>

#ifdef DEBUG
const bool USE_SERIAL = true;
#else
const bool USE_SERIAL = false;
#endif

void setup_serial(int baud) {
    if (!USE_SERIAL)
        return;

    Serial.begin(baud);
    Serial.println(); // init monitor
    Serial.println(F("Welcome to Verdandi"));
    Serial.println(F("Debug mode is activated"));
}

void heartbeat_serial() {
    if (!USE_SERIAL)
        return;

    static uint32_t last_msg = 0;
    const uint16_t time_interval = 5 * 1000;

    uint32_t time = millis();

    if (time - last_msg > time_interval) {
        Serial.print(F("."));
        last_msg = time;
    }
}

String readString(uint8_t expected_length = 1) {
    if (!USE_SERIAL)
        return "";

    if (Serial.available() < expected_length)
        return "";

    // read the incoming:
    String incoming = Serial.readString();
    // invisible chars will drive you mad while debugging
    incoming.trim();

    // say what you got:
    print(F("Received:"));
    printlnRaw(incoming);

    return incoming;
}

// protected method to enforce "F" and ensure flag usage
void println(const __FlashStringHelper* string) {
    if (!USE_SERIAL)
        return;
    Serial.println(string);
}

void printlnRaw(String string) {
    if (!USE_SERIAL)
        return;
    Serial.println(string);
}

void printlnRaw(uint16_t number) {
    if (!USE_SERIAL)
        return;
    Serial.println(number);
}

void print(const __FlashStringHelper* string) {
    if (!USE_SERIAL)
        return;
    Serial.print(string);
}

void printRaw(String string) {
    if (!USE_SERIAL)
        return;
    Serial.print(string);
}

void printRaw(uint16_t number) {
    if (!USE_SERIAL)
        return;
    Serial.print(number);
}
