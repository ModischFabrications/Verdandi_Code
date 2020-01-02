#pragma once

#include "network/timeService.h"
#include "persistence/PersistenceManager.h"
#include "serial/SerialWrapper.h"

namespace SerialInputTest {

void handleInput() {
    String received = readString(1);
    if (received == "I") {
        println(F("Resetting config"));
        PersistenceManager::set(defaultConfiguration);
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
        TimeService::timeUpdate();
    }
}

} // namespace SerialInputTest
