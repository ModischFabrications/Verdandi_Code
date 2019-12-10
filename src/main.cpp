//#include <Arduino.h>
#include <ESP8266WiFi.h> // we have WiFi
#include <WiFiManager.h> // captive portal

// define before importing
#define DEBUG
#include "serialWrapper.h"

// could (should?) be moved into other files
const uint8_t PIN_RGB = D1;
const uint8_t N_LEDS = 24;

WiFiManager wifiManager;

//gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager *myWiFiManager)
{
    println(F("Entered config mode"));
    // auto generated SSID might be unknown
    printlnRaw(myWiFiManager->getConfigPortalSSID());
    printlnRaw(WiFi.softAPIP().toString());
}

void setup_WiFi()
{
    println(F("setting up wifi (with captive portal)"));

    // DEBUGGING:
    // wifiManager.resetSettings();

    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
    //wifiManager.setConfigPortalTimeout(60);

    // TODO: while? with blinking
    if (!wifiManager.autoConnect("Verdandi"))
    {
        println(F("failed to connect and hit timeout"));
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(1000);
    }

    //if you get here you have connected to the WiFi
    println(F("connected...yeey :)"));
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    // TODO: abstract away pullup/-down. BUILTIN is inverted (LOW -> ON)
    digitalWrite(LED_BUILTIN, LOW);
    
    setup_serial(9600);
    setup_WiFi();

    // ...

    digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
    delay(100);

    heartbeat_serial();
}
