#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "SerialWrapper.h"
#include "configuration.h"
#include "persistenceStore.h"

const uint16_t delay_to_save_ms = (5 * 1000);

// classes don't behave well with pointers!

// singleton, we have only one EEPROM
class PersistenceManager {
  private:
    static Configuration configuration;
    static uint32_t t_next_savepoint; // =0 impossible here!

    // fully static class, no constructor allowed
    PersistenceManager() {}

  public:
    // not threadsafe, but whatever
    static Configuration get() {
      // impossible!
        if (!configuration) {
            configuration = load_settings();
        }

        return configuration;
    }
    // persistent only after a small timeout
    static void set(Configuration& new_config) {
        if (configuration == new_config) {
            return;
        }

        // set "moving" timer to save as soon as user is done
        t_next_savepoint = (millis() + delay_to_save_ms);
    }

    /**
     * Call regularly to persist settings to EEPROM.
     * saves configuration to EEPROM if timeout has passed.
     *
     * TODO: roll-over protection
     * */
    static void try_save() {
        // TODO: is this safe with overflowing values (> 1 day)?
        if (t_next_savepoint != 0 && millis() >= t_next_savepoint) {
            save_settings(configuration);
            t_next_savepoint = 0;

            if (USE_SERIAL) {
                println(F("Saving to EEPROM"));
            }
        }
    }
};

Configuration PersistenceManager::configuration = nullptr;
uint32_t PersistenceManager::t_next_savepoint = 0;
