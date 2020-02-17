#pragma once

#define FASTLED_INTERNAL // disable pragma message
#include <FastLED.h>
#include <math.h>

#include "persistence/configuration.h"
#include "persistence/persistenceManager.h"

#include "network/timeService.h"

namespace LedController {

enum State { INIT = 0, RUNNING, NIGHTMODE };
State state = State::INIT;

// hidden globals
namespace {
const uint16_t T_DEFAULT_FLASH_DURATION_MS = (0.2 * 1000);
const uint16_t T_ANIMATION_MS = (1 * 1000);

const uint16_t MAX_MILLIAMPS = 1000;

const uint8_t PIN_LEDS = D1;
const uint8_t N_LEDS = 24;
// the index of the led that represents 12 o'clock
const uint8_t I_LED_12H = 12;

const CRGB C_OK = CRGB::Green;
const CRGB C_WARN = CRGB::Yellow;
const CRGB C_ERR = CRGB::Red;

const bool INTERPOLATE = false;

CRGB leds[N_LEDS];
float multipliersHour[N_LEDS];
float multipliersMinute[N_LEDS];
float multipliersSecond[N_LEDS];
float previousMultipliersHour[N_LEDS];
float previousMultipliersMinute[N_LEDS];
float previousMultipliersSecond[N_LEDS];

void updateDisplay();
void clearMultipliers();
void setMultiplier(float multipliers[N_LEDS], float clockProgress, uint8_t timeDivider);
void writeLeds(uint8_t colorH[3], uint8_t colorM[3], uint8_t colorS[3]);
void interpolateLeds();
bool shouldBeNightMode(Time currentTime);
void checkNightMode(Time currentTime);

// -----------------------

void updateDisplay(Time currentTime) {
    clearMultipliers();

    // show clock hand as fraction
    float h = (float)currentTime.hour + (float)currentTime.minute / 60.0;
    float m = (float)currentTime.minute + (float)currentTime.second / 60.0;
    float s = (float)currentTime.second + (float)currentTime.millisecond / 1000.0;

    Config::Configuration config = PersistenceManager::get();
    if (config.showHours)
        setMultiplier(multipliersHour, h, 12);
    if (config.showMinutes)
        setMultiplier(multipliersMinute, m, 60);
    if (config.showSeconds)
        setMultiplier(multipliersSecond, s, 60);

    // generate normalized multipliers
    for (uint8_t i = 0; i < N_LEDS; i++) {
        float sum = multipliersHour[i] + multipliersMinute[i] + multipliersSecond[i];
        // if adding up the color multipliers is greater than one than they need to be normalized
        if (sum > 1.0) {
            multipliersHour[i] /= sum;
            multipliersMinute[i] /= sum;
            multipliersSecond[i] /= sum;
        }
    }

    if(INTERPOLATE) {
        interpolateLeds();
    }

    writeLeds(config.colorH, config.colorM, config.colorS);
}

/**
 * Reset led multipliers.
 */
void clearMultipliers() {
    for (uint8_t i = 0; i < N_LEDS; i++) {
        multipliersHour[i] = 0;
        multipliersMinute[i] = 0;
        multipliersSecond[i] = 0;
    }
}

/**
 * Write sub-division value multipliers.
 * @param multipliers array to modify
 * @param clockProgress 0..timeDivider
 * @param timeDivider e.g. 24 or 60
 */
void setMultiplier(float multipliers[N_LEDS], float clockProgress, uint8_t timeDivider) {
    while (clockProgress > timeDivider)
        clockProgress -= timeDivider;

    // 0..1
    float percentage = clockProgress / (float)timeDivider;

    uint8_t firstActiveLed = floor(percentage * (float)N_LEDS);

    multipliers[firstActiveLed] = 1.0 - (percentage * (float)N_LEDS - (float)firstActiveLed);
    multipliers[(firstActiveLed + 1) % N_LEDS] = 1.0 - multipliers[firstActiveLed];
}

/**
 * Set the color for every led according to the colors multiplied with the multipliers.
 * @param colorH color for the hours hand
 * @param colorM color for the minutes hand
 * @param colorS color for the seconds hand
 */
void writeLeds(uint8_t colorH[3], uint8_t colorM[3], uint8_t colorS[3]) {
    for (uint8_t i = 0; i < N_LEDS; i++) {
        CRGB ledColorH = CRGB(colorH[0], colorH[1], colorH[2]);
        uint8_t normalizedMultiplierHour = 256 * multipliersHour[i];
        ledColorH.nscale8_video(normalizedMultiplierHour);

        CRGB ledColorM = CRGB(colorM[0], colorM[1], colorM[2]);
        uint8_t normalizedMultiplierMinute = 256 * multipliersMinute[i];
        ledColorM.nscale8_video(normalizedMultiplierMinute);

        CRGB ledColorS = CRGB(colorS[0], colorS[1], colorS[2]);
        uint8_t normalizedMultiplierSecond = 256 * multipliersSecond[i];
        ledColorS.nscale8_video(normalizedMultiplierSecond);

        leds[(i + I_LED_12H) % N_LEDS] = ledColorH + ledColorM + ledColorS;
    }
    FastLED.show();
}

/**
 * Smooths the led multipliers by interpolating with the previous value.
 */
void interpolateLeds() {
    for(uint8_t i = 0; i < N_LEDS; ++i) {
        multipliersHour[i] = 0.5 * (multipliersHour[i] + previousMultipliersHour[i]);
        previousMultipliersHour[i] = multipliersHour[i];
        multipliersMinute[i] = 0.5 * (multipliersMinute[i] + previousMultipliersMinute[i]);
        previousMultipliersMinute[i] = multipliersMinute[i];
        multipliersSecond[i] = 0.5 * (multipliersSecond[i] + previousMultipliersSecond[i]);
        previousMultipliersSecond[i] = multipliersSecond[i];
    }


}

bool shouldBeNightMode(Time currentTime) {
    Config::Configuration config = PersistenceManager::get();

    if (!config.nightmode)
        return false;

    if (currentTime > config.turnOffAt || currentTime < config.turnOnAt)
        return true;

    return false;
}

void checkNightMode(Time currentTime) {
    if (shouldBeNightMode(currentTime) && state != NIGHTMODE) {
        println(F("Switching to NIGHTMODE"));
        // turn off LEDs on transition to prevent stuck LEDs
        fill_solid(leds, N_LEDS, CRGB::Black);
        FastLED.show();

        state = NIGHTMODE;
    } else if (!shouldBeNightMode(currentTime) && state == NIGHTMODE) {
        println(F("Switching to RUNNING"));
        state = RUNNING;
    }
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
    // TODO: display error while state == INIT, will probably need 
    // a INVALID state in time or a listener
    if (state == INIT) {
        println(F("Switching INIT to RUNNING"));
        state = RUNNING;
    }

    Time currentTime = TimeService::getCurrentTime();

    // TODO: execute some functions only every x cycles
    // --> check nightmode by minute, init probably with higher refresh rate
    // --> scheduler? callEveryMinute, callEverySecond
    checkNightMode(currentTime);

    // TODO: implement dimming when near night time
    // turn off LEDs and skip updating outside of working hours
    if (state == RUNNING)
        updateDisplay(currentTime);
}

void updateConfiguration() {
    // color does not need to be cached here, read every time from config
    Config::Configuration config = PersistenceManager::get();
    print(F("Updating brightness to "));
    printlnRaw(config.brightness);
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