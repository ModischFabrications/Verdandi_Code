#pragma once

#include <Arduino.h>

#ifdef DEBUG
const bool USE_SERIAL = true;
#else
const bool USE_SERIAL = false;
#endif

void setupSerial(int baud) {
    if (!USE_SERIAL)
        return;

    Serial.begin(baud);
    Serial.println(); // init monitor
    Serial.println(F("Welcome to Verdandi"));
    Serial.println(F("Debug mode is activated"));
}

void heartbeatSerial() {
    if (!USE_SERIAL)
        return;

    static uint32_t lastMsg = 0;
    const uint16_t timeInterval = 5 * 1000;

    uint32_t time = millis();

    if (time - lastMsg > timeInterval) {
        Serial.print(F("."));
        lastMsg = time;
    }
}

String readString(uint8_t expectedLength = 1) {
    if (!USE_SERIAL)
        return "";

    if (Serial.available() < expectedLength)
        return "";

    // read the incoming:
    String incoming = Serial.readString();
    // invisible chars will drive you mad while debugging
    incoming.trim();

    // say what you got:
    Serial.print(F("Received:"));
    Serial.println(incoming);

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
