#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "serial/SerialWrapper.h"

#include "persistence/configuration.h"
#include "persistence/persistenceStore.h"

// definition for all handlers
typedef void (*fListener)();

// classes don't behave well with pointers, trust me!
namespace PersistenceManager {

Configuration get();
void set(Configuration newConfig);
void trySave();
void registerListener(fListener listener);
void callListeners();

// anonymous namespace hides globals towards other
namespace {
const uint8_t N_MAX_LISTENERS = 7;

const uint16_t delayToSaveMs = (5 * 1000);

// we are unable to determine if a variable was initialized and
//  we don't want to define a "null" Configuration as default
bool initialized = false;

// it's dangerous to leave this uninitialized but we get these values
//  with the first get()
Configuration configuration;
uint32_t tNextSavepoint = 0;

fListener listeners[N_MAX_LISTENERS] = {nullptr};
uint8_t i_listeners = 0;
} // namespace

// get with included lazy load from EEPROM
Configuration get() {
    // singleton-like
    if (!initialized) {
        println(F("Loading initial config from EEPROM"));
        configuration = PersistenceStore::loadSettings();
        initialized = true;
        callListeners();
    }

    return configuration;
}

// set with lazy save, persistent only after a small timeout to reduce EEPROM
// wear
void set(Configuration newConfig) {
    if (configuration == newConfig) {
        println(F("config identical, skipping save"));
        return;
    }

    // TODO: copy or reassign? Reference could be tricky
    configuration = newConfig;

    // set "moving" timer to save as soon as user is done
    tNextSavepoint = (millis() + delayToSaveMs);

    // TODO: notify every listener of PersistenceManager that the values changed?
    callListeners();
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

        println(F("Saving to EEPROM"));
    }
}

void registerListener(fListener listener) {
    println(F("Adding listener"));

    if (i_listeners >= N_MAX_LISTENERS) {
        println(F("List is full, unable to add listener"));
    }

    listeners[i_listeners++] = listener;
}

void callListeners() {
    for (uint8_t i = 0; i < i_listeners; i++) {
        printlnRaw("Calling listener " + i);

        fListener listener = listeners[i];

        // check for value
        if (listener == NULL) {
            println(F("Listener not initialised"));
        }
        // unpack function pointer from list and call
        (*listener)();
    }
}

} // namespace PersistenceManager
