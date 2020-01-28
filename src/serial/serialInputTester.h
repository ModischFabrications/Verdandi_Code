#pragma once

#include "network/timeService.h"
#include "persistence/PersistenceManager.h"
#include "serial/SerialWrapper.h"
#include "led/ledController.h"

namespace SerialInputTest {

void handleInput() {
    String received = readString(1);
    if (received == "?") {
        println(F("?, I, L, M, T, G"));
    }
    if (received == "I") {
        println(F("Resetting config"));
        PersistenceManager::set(Config::defaultConfiguration);
    }
    if (received == "L") {
        println(F("Loading config"));
        Config::Configuration config = PersistenceManager::get();
        printlnRaw(config.brightness);
    }
    if (received == "M") {
        println(F("Modifying config"));
        Config::Configuration config = PersistenceManager::get();
        config.brightness += 8;
        PersistenceManager::set(config);
    }
    if (received == "T") {
        TimeService::timeUpdate();
    }
    if (received == "G") {
        print(F("LED State:"));
        printlnRaw(LedController::state);
    }
}

} // namespace SerialInputTest
