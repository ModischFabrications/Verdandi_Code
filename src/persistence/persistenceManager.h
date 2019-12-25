#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "SerialWrapper.h"
#include "configuration.h"
#include "persistenceStore.h"

// classes don't behave well with pointers, trust me!
namespace PersistenceManager {

// anonymous namespace hides globals towards other
namespace {
// TODO: set to realistic value (5s?)
const uint16_t delay_to_save_ms = (2 * 1000);

// we are unable to determine if a variable was initialized and
//  we don't want to define a "null" Configuration as default
bool initialized = false;

// it's dangerous to leave this uninitialized but we get these values
//  with the first get()
Configuration configuration;
uint32_t t_next_savepoint = 0;
} // namespace

// get with included lazy load from EEPROM
Configuration get() {
    // singleton-like
    if (!initialized) {
        println(F("Loading initial config from EEPROM"));
        configuration = load_settings();
        initialized = true;
    }

    return configuration;
}

// set with lazy save, persistent only after a small timeout to reduce EEPROM
// wear
void set(Configuration& new_config) {
    if (configuration == new_config) {
        println(F("config identical, skipping save"));
        return;
    }

    // TODO: copy or reassign? Reference could be tricky
    configuration = new_config;

    // set "moving" timer to save as soon as user is done
    t_next_savepoint = (millis() + delay_to_save_ms);
}

/**
 * Call regularly to persist settings to EEPROM.
 * saves configuration to EEPROM if timeout has passed.
 *
 * TODO: roll-over protection
 * */
void try_save() {
    // TODO: is this safe with overflowing values (> 1 day)?
    if (t_next_savepoint != 0 && millis() >= t_next_savepoint) {
        save_settings(configuration);
        t_next_savepoint = 0;

        if (USE_SERIAL) {
            println(F("Saving to EEPROM"));
        }
    }
}

} // namespace PersistenceManager
