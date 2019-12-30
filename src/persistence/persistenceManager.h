#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "serial/SerialWrapper.h"

#include "persistence/configuration.h"
#include "persistence/persistenceStore.h"

// classes don't behave well with pointers, trust me!
namespace PersistenceManager {

// anonymous namespace hides globals towards other
namespace {
// TODO: set to realistic value (5s?)
const uint16_t delayToSaveMs = (2 * 1000);

// we are unable to determine if a variable was initialized and
//  we don't want to define a "null" Configuration as default
bool initialized = false;

// it's dangerous to leave this uninitialized but we get these values
//  with the first get()
Configuration configuration;
uint32_t tNextSavepoint = 0;
} // namespace

// get with included lazy load from EEPROM
Configuration get() {
    // singleton-like
    if (!initialized) {
        println(F("Loading initial config from EEPROM"));
        configuration = PersistenceStore::loadSettings();
        initialized = true;
    }

    return configuration;
}

// set with lazy save, persistent only after a small timeout to reduce EEPROM
// wear
void set(Configuration& newConfig) {
    if (configuration == newConfig) {
        println(F("config identical, skipping save"));
        return;
    }

    // TODO: copy or reassign? Reference could be tricky
    configuration = newConfig;

    // set "moving" timer to save as soon as user is done
    tNextSavepoint = (millis() + delayToSaveMs);
}

/**
 * Call regularly to persist settings to EEPROM.
 * saves configuration to EEPROM if timeout has passed.
 *
 * TODO: roll-over protection
 * */
void trySave() {
    // TODO: is this safe with overflowing values (> 1 day)?
    if (tNextSavepoint != 0 && millis() >= tNextSavepoint) {
        PersistenceStore::saveSettings(configuration);
        tNextSavepoint = 0;

        if (USE_SERIAL) {
            println(F("Saving to EEPROM"));
        }
    }
}

} // namespace PersistenceManager
