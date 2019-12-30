#pragma once

// https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM
#include <EEPROM.h>

#include "persistence/configuration.h"

namespace PersistenceStore {
// Preserve EEPROM while programming with the EESAVE fuse on AVR

namespace {
/* change with each design iteration to prevent EEPROM inconsistency and help
 * with wear leveling of EEPROM cells.
 * The chance that a random, unset combination is a match is very low.
 */
const uint8_t VERSION = 2;

/**
 * EEPROM has around 100k writes per cell, so use them carefully!
 * It might be a good idea to switch to new cells with each revision.
 * ATtiny has 512 bytes EEPROM.
 *
 * Wwear leveling is logically impossible to automate without external
 * input, e.g. version numbers.
 * Storage pointers burn out the pointer cell, random won't be found
 * after a restart.
 * */
const uint16_t EEPROM_VERSION_ADDR = VERSION;
const uint16_t EEPROM_SETTINGS_ADDR = EEPROM_VERSION_ADDR + sizeof(VERSION);

const uint16_t MAX_BYTES_USED = 127;
} // namespace

// call once to reserve (emulated) storage for usage
void setup() {
    // ESP needs to know how much storage we actually need, AVR won't
    EEPROM.begin(MAX_BYTES_USED);
}

/**
 * save settings to EEPROM for persistent storage
 * */
void save_settings(Configuration settings) {
    EEPROM.write(EEPROM_VERSION_ADDR, VERSION);
    EEPROM.put(EEPROM_SETTINGS_ADDR, settings);

    // commit planned changes
    EEPROM.commit();
}

/**
 * load settings from EEPROM, this will reset and return default values for
 * corrupted, outdated or missing settings.
 * */
Configuration load_settings() {
    // could be rubbish or zeros, either way should work
    uint8_t saved_version = EEPROM.read(EEPROM_VERSION_ADDR);

    if (saved_version != VERSION) {
        // save new default settings as fallback, updates version for next run
        save_settings(default_configuration);

        return default_configuration;
    }

    // content should be correct, return it
    Configuration settings;
    EEPROM.get(EEPROM_SETTINGS_ADDR, settings);

    return settings;
}

} // namespace PersistenceStore