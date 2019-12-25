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
    //TODO: set to realistic value (5s?)
const uint16_t delay_to_save_ms = (1 * 1000);

bool unsaved = true;
// it's dangerous to leave this uninitialized but we get these values 
//  with the first get()
Configuration configuration;
uint32_t t_next_savepoint = 0;
} // namespace


Configuration get() {
    if (unsaved) {
        println(F("Loading initial config form EEPROM"));
        configuration = load_settings();
    }

    return configuration;
}

// persistent only after a small timeout
void set(Configuration& new_config) {
    if (configuration == new_config) {
        println(F("config identical, skipping save"));
        return;
    }

    unsaved = true;
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
        unsaved = false;

        if (USE_SERIAL) {
            println(F("Saving to EEPROM"));
        }
    }
}

} // namespace PersistenceManager
