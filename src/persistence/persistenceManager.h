#pragma once

#include <EEPROM.h>

#include "configuration.h"

/**
 * EEPROM has around 100k writes per cell, so use them carefully!
 * It might be a good idea to switch to new cells with each revision.
 * ATtiny has 512 bytes EEPROM.
 * 
 * TODO: Preserve EEPROM while programming -> EESAVE fuse
 * */
#define EEPROM_VERSION_ADDR 12
#define EEPROM_SETTINGS_ADDR 20

// change with each design iteration to prevent EEPROM inconsistency
// the chance that a random combination is a match is very low
const uint8_t version = 2;

// -----------

/**
 * save settings to EEPROM for persistent storage
 * TODO: check if convenience features actually use 2 bytes as before
 * */
void save_settings(Configuration settings)
{
  EEPROM.put(EEPROM_VERSION_ADDR, version);
  EEPROM.put(EEPROM_SETTINGS_ADDR, settings);
}

/**
 * load settings from EEPROM, this will reset and return default values for
 * corrupted, outdated or missing settings.
 * */
Configuration load_settings()
{
  uint8_t saved_version;
  EEPROM.get(EEPROM_VERSION_ADDR, saved_version);

  if (saved_version != version)
  {
    // save new default settings, updates version
    save_settings(default_configuration);

    return default_configuration;
  }

  // content should be correct, return it
  Configuration settings;
  EEPROM.get(EEPROM_SETTINGS_ADDR, settings);

  return settings;
}
