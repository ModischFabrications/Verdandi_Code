//#include <Arduino.h>
#include <ESP8266WiFi.h>    // we have WiFi
#include <WiFiManager.h>    // captive portal

//#include <HardwareSerial.h>

const bool USE_SERIAL = true;

// could (should?) be moved into other files
const uint8_t PIN_RGB = D1;
const uint8_t N_LEDS = 24;

void serial_setup() {
    if (!USE_SERIAL) return;

    Serial.begin(9600);
    Serial.println();
    Serial.println(F("Welcome to Verdandi"));
    Serial.println(F("Debug mode is activated"));
}

void serial_heartbeat() {
    if (!USE_SERIAL) return;
    static uint32_t last_msg = 0;
    const uint16_t time_interval = 5 * 1000;

    uint32_t time = millis();

    if (time - last_msg > time_interval) {
        Serial.println(F("."));
        last_msg = time;
    }
}


void setup() {
    serial_setup();

}

void loop() {
    delay(100);

    serial_heartbeat();
}


