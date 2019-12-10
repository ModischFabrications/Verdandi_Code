//#include <Arduino.h>
#include <ESP8266WiFi.h> // we have WiFi
#include <WiFiManager.h> // captive portal

//#include <HardwareSerial.h>

const bool USE_SERIAL = true;

// could (should?) be moved into other files
const uint8_t PIN_RGB = D1;
const uint8_t N_LEDS = 24;


void setup_serial()
{
    if (!USE_SERIAL)
        return;

    Serial.begin(9600);
    Serial.println();
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


void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    // TODO: abstract away pullup/-down
    digitalWrite(LED_BUILTIN, LOW);
    setup_serial();

    // ...

    digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
    delay(100);

    heartbeat_serial();
}

