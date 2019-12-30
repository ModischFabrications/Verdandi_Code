#pragma once

#include "SerialWrapper.h"
#include "persistence/PersistenceManager.h"
#include "network/ntp.h"

namespace SerialInputTest {

void handleInput() {
    // TODO: remove or extract
    String received = readString(1);
    if (received == "I") {
        println(F("Resetting config"));
        PersistenceManager::set(default_configuration);
    }
    if (received == "L") {
        println(F("Loading config"));
        Configuration config = PersistenceManager::get();
        printlnRaw(config.brightness);
    }
    if (received == "S") {
        println(F("Modifying config"));
        Configuration config = PersistenceManager::get();
        config.brightness += 8;
        PersistenceManager::set(config);
    }
    if (received == "T") {
        timeUpdate();
    }
}

} // namespace SerialInputTest
