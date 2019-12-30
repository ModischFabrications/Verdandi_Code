#pragma once

#include <FastLED.h>

#include "persistence/configuration.h"
#include "persistence/persistenceManager.h"

namespace LedController {

void flash(CRGB color, uint16_t duration = T_DEFAULT_FLASH_DURATION);

void setup();
void helloPower();
void tick();

void updateConfiguration();

// hidden globals
namespace {
const uint16_t T_DEFAULT_FLASH_DURATION = (0.2 * 1000);

const uint16_t MAX_MILLIAMPS = 1000;

const uint8_t PIN_LEDS = D1;
const uint8_t N_LEDS = 24;

const CRGB C_OK = CRGB::Green;
const CRGB C_WARN = CRGB::Yellow;
const CRGB C_CRIT = CRGB::Red;

CRGB leds[N_LEDS];

} // namespace

void setup() {
    pinMode(PIN_LEDS, OUTPUT);

    FastLED.addLeds<WS2812B, PIN_LEDS, GRB>(leds, N_LEDS);
    // set powerlimit to 5v, 1000mA (Fuse size)
    FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
}

void helloPower() {
    // TODO: run around to check every LED (and make it look cool)
}

void tick() {
    // TODO: wipe stripe, call "setIndicator(position, color)" x3
}

void updateConfiguration() {
    Configuration config = PersistenceManager::get();
    FastLED.setBrightness(config.brightness);
    // color does not need to be cached here, read every time from config

    // TODO: hide/show indicators -> enable "writeSeconds"
}

/**
 * quick flash to signal something.
 * Designed for debug use.
 * */
void flash(CRGB color, uint16_t duration = T_DEFAULT_FLASH_DURATION) {
    // decrease to prevent burning out your eyes while debugging
    uint8_t prevBrightness = FastLED.getBrightness();
    FastLED.setBrightness(32);

    fill_solid(leds, N_LEDS, color);
    FastLED.show();

    // This could be better but it's not much more difficult to do it async and clean.
    // this->t_lock_output_until = (millis() + duration);

    delay(duration * 3 / 4);

    // reset everything possible
    FastLED.setBrightness(prevBrightness);
    fill_solid(leds, N_LEDS, CRGB::Black); // restore previous?
    FastLED.show();

    // make sure that OFF is visible in every situation
    delay(duration * 1 / 4);

    // full reset with values is happening on next reading(display_level)
}

} // namespace LedController