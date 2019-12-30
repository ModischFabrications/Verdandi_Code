#pragma once

#define FASTLED_INTERNAL // disable pragma message
#include <FastLED.h>

#include "persistence/configuration.h"
#include "persistence/persistenceManager.h"

namespace LedController {

struct Time {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

// hidden globals
namespace {
const uint16_t T_DEFAULT_FLASH_DURATION_MS = (0.2 * 1000);
const uint16_t T_ANIMATION_MS = (1 * 1000);

const uint16_t MAX_MILLIAMPS = 1000;

const uint8_t PIN_LEDS = D1;
const uint8_t N_LEDS = 24;

const CRGB C_OK = CRGB::Green;
const CRGB C_WARN = CRGB::Yellow;
const CRGB C_ERR = CRGB::Red;

CRGB leds[N_LEDS];

} // namespace

void flash(CRGB color, uint16_t duration = T_DEFAULT_FLASH_DURATION_MS);

void setup();
void helloPower();
void tick();

void updateConfiguration();

// -------------------

void setup() {
    pinMode(PIN_LEDS, OUTPUT);

    FastLED.addLeds<WS2812B, PIN_LEDS, GRB>(leds, N_LEDS);
    // set powerlimit to 5v, 1000mA (Fuse size)
    FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);
}

void helloPower() {
    // TODO: use fading, might want to reuse clock functions
    // might even want to use a rainbow fade? -> check FastLED buildin animations
    for (uint8_t i = 0; i < N_LEDS; i++) {
        leds[i] = CRGB::White;
        FastLED.show();
        delay(T_ANIMATION_MS / N_LEDS); // 1 second for whole bar
        leds[i] = CRGB::Black;
    }
}

void tick() {
    // TODO: wipe stripe, call "setIndicator(position, color)" x3
}

void setTime(Time time) {
    // TODO: sync internal millis offset calculation with real time
    // slow integration (like Visu3D.sync_factor) to prevent jumps
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
void flash(CRGB color, uint16_t duration) {
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