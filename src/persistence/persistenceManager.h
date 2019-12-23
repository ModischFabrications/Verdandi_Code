#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "SerialWrapper.h"
#include "configuration.h"
#include "persistenceStore.h"

const uint16_t delay_to_save_ms = (5 * 1000);

// classes don't behave well with pointers!

class PersistenceManager {
  private:
    uint32_t t_next_savepoint = 0;

    PersistenceManager() {}

  public:
    static const PersistenceManager instance;
    // not threadsafe, but whatever
    Configuration configuration;

    void apply(Configuration& new_config) {
        if (this->configuration == new_config) {
            return;
        }

        // set "moving" timer to save as soon as user is done
        this->t_next_savepoint = (millis() + delay_to_save_ms);
    }

    /**
     * Call regularly to persist settings to EEPROM.
     * saves configuration to EEPROM if timeout has passed.
     *
     * TODO: roll-over protection
     * */
    void try_save() {
        // TODO: is this safe with overflowing values (> 1 day)?
        if (this->t_next_savepoint != 0 && millis() >= this->t_next_savepoint) {
            save_settings(this->configuration);
            this->t_next_savepoint = 0;

            if (USE_SERIAL) {
                println(F("Saving to EEPROM"));
            }
        }
    }
};
