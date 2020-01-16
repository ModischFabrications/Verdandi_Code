#pragma once

#define FASTLED_INTERNAL // disable pragma message
#include <FastLED.h>
#include <math.h>

#include "persistence/configuration.h"
#include "persistence/persistenceManager.h"

#include "network/timeService.h"


namespace LedController {

// hidden globals
namespace {
const uint16_t T_DEFAULT_FLASH_DURATION_MS = (0.2 * 1000);
const uint16_t T_ANIMATION_MS = (1 * 1000);

const uint16_t MAX_MILLIAMPS = 1000;

const uint8_t PIN_LEDS = D1;
const uint8_t N_LEDS = 24;
// the index of the led that represents 12 o'clock
const uint8_t I_LED_12H = 0;

const CRGB C_OK = CRGB::Green;
const CRGB C_WARN = CRGB::Yellow;
const CRGB C_ERR = CRGB::Red;

CRGB leds[N_LEDS];
float preMultipliersHour[N_LEDS];
float preMultipliersMinute[N_LEDS];
float preMultipliersSecond[N_LEDS];

void updateDisplay();
void clearPreMultipliers();
void setPreMultiplier(float multipliers[N_LEDS], float clockProgress, uint8_t timeDivider);
void writeLeds(uint8_t colorH[3], uint8_t colorM[3], uint8_t colorS[3]);
// -----------------------

void updateDisplay(Time currentTime) {
    clearPreMultipliers();

    // show clock hand as fraction
    float h = (float)currentTime.hour + (float)currentTime.minute / 60.0;
    float m = (float)currentTime.minute + (float)currentTime.second / 60.0;
    float s = (float)currentTime.second + (float)currentTime.millisecond / 1000.0;

    Configuration config = PersistenceManager::get();
    if (config.showHours)
        setPreMultiplier(preMultipliersHour, h, 12);
    if (config.showMinutes)
        setPreMultiplier(preMultipliersMinute, m, 60);
    if (config.showSeconds)
        setPreMultiplier(preMultipliersSecond, s, 60);

    // generate normalized multipliers
    for (uint8_t i = 0; i < N_LEDS; i++) {
        float sum = preMultipliersHour[i] + preMultipliersMinute[i] + preMultipliersSecond[i];
        // if adding up the color multipliers is greater than one than they need to be normalized
        if (sum > 1.0) {
            preMultipliersHour[i] /= sum;
            preMultipliersMinute[i] /= sum;
            preMultipliersSecond[i] /= sum;
        }
    }

    writeLeds(config.colorH, config.colorM, config.colorS);
}

void clearPreMultipliers() {
    for (uint8_t i = 0; i < N_LEDS; i++) {
        preMultipliersHour[i] = 0;
        preMultipliersMinute[i] = 0;
        preMultipliersSecond[i] = 0;
    }
}

void setPreMultiplier(float multipliers[N_LEDS], float clockProgress, uint8_t timeDivider) {
    while (clockProgress > timeDivider)
        clockProgress -= timeDivider;
    float percentage = clockProgress / (float)timeDivider;

    uint8_t firstLed = floor(percentage * N_LEDS);

    multipliers[firstLed] = 1 - (float)(percentage * N_LEDS - firstLed);
    multipliers[(firstLed + 1) % N_LEDS] = 1 - multipliers[firstLed];
}

void writeLeds(uint8_t colorH[3], uint8_t colorM[3], uint8_t colorS[3]) {
    for (uint8_t i = 0; i < N_LEDS; i++) {
        CRGB ledColorH = CRGB(colorH[0], colorH[1], colorH[2]);
        fract8 normalizedMultiplierHour = 256 * preMultipliersHour[i];
        ledColorH.nscale8_video(normalizedMultiplierHour);

        CRGB ledColorM = CRGB(colorM[0], colorM[1], colorM[2]);
        fract8 normalizedMultiplierMinute = 256 * preMultipliersMinute[i];
        ledColorM.nscale8_video(normalizedMultiplierMinute);

        CRGB ledColorS = CRGB(colorS[0], colorS[1], colorS[2]);
        fract8 normalizedMultiplierSecond = 256 * preMultipliersSecond[i];
        ledColorS.nscale8_video(normalizedMultiplierSecond);

        leds[(i + I_LED_12H) % N_LEDS] = ledColorH + ledColorM + ledColorS;
    }
    FastLED.show();
}
} // namespace

void setup();
void helloPower();
void tick();
void flash(CRGB color, uint16_t duration = T_DEFAULT_FLASH_DURATION_MS);
void updateConfiguration();

// -----------------------

void setup() {
    pinMode(PIN_LEDS, OUTPUT);

    FastLED.addLeds<WS2812B, PIN_LEDS, GRB>(leds, N_LEDS);
    // set powerlimit to 5v, 1000mA (Fuse size)
    FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_MILLIAMPS);

    PersistenceManager::registerListener(updateConfiguration);
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
    Time currentTime = TimeService::getCurrentTime();

    // TODO: turn off LEDs and skip updating outside of working hours

    updateDisplay(currentTime);
}

void updateConfiguration() {
    // color does not need to be cached here, read every time from config
    Configuration config = PersistenceManager::get();
    FastLED.setBrightness(config.brightness);
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