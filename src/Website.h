#pragma once

#include <Arduino.h>

// TODO: make constant variable
//const __FlashStringHelper* content = F("Hallo\nWorld");

const __FlashStringHelper* getContent() {
    return F("Hello\nWorld");
}
