#pragma once

#include <stdint.h>

#include "Time.h"

namespace Config {

const uint8_t N_TIMEZONE = 50;
const uint8_t N_TIMEZONE_NAME = 40;


/**
 * change with each design iteration to prevent EEPROM inconsistency and help
 * with wear leveling of EEPROM cells.
 * */
const uint8_t VERSION = 4;

struct Configuration {
    uint8_t brightness;

    bool showHours;
    bool showMinutes;
    bool showSeconds;

    // irrelevant if "!showX"
    uint8_t colorH[3];
    uint8_t colorM[3];
    uint8_t colorS[3];

    uint16_t pollInterval;

    bool nightmode;

    // irrelevant if "!nightmode"
    Time turnOffAt;
    Time turnOnAt;

    // name is needed to keep a bijective mapping
    char timezone[N_TIMEZONE];
    char timezoneName[N_TIMEZONE_NAME];

    bool useFading;

    const bool operator==(const Configuration& other) {
        return (this->brightness == other.brightness &&
                this->showHours == other.showHours &&
                this->showMinutes == other.showMinutes &&
                this->showSeconds == other.showSeconds &&
                this->colorH[0] == other.colorH[0] &&
                this->colorH[1] == other.colorH[1] &&
                this->colorH[2] == other.colorH[2] &&
                this->colorM[0] == other.colorM[0] &&
                this->colorM[1] == other.colorM[1] &&
                this->colorM[2] == other.colorM[2] &&
                this->colorS[0] == other.colorS[0] &&
                this->colorS[1] == other.colorS[1] &&
                this->colorS[2] == other.colorS[2] &&
                this->pollInterval == other.pollInterval &&
                this->nightmode == other.nightmode &&
                this->turnOffAt == other.turnOffAt &&
                this->turnOnAt == other.turnOnAt &&
                (strcmp(this->timezone, other.timezone) == 0) &&
                (strcmp(this->timezoneName, other.timezoneName) == 0) &&
                this->useFading == other.useFading
                );
    }
};

Configuration defaultConfiguration = {
    127,
    true, true, true,
    {255, 0, 0}, {0, 255, 0}, {0, 0, 255},
    3 * 60,
    false, {10}, {11},
    "GMT0",
    "GMT",
    true
    };

} // namespace Configuration