#pragma once
#include <stdint.h>

struct Configuration {
    uint8_t brightness;

    bool showHours;
    bool showMinutes;
    bool showSeconds;

    uint8_t colorH[3];
    // irrelevant if "!showX"
    uint8_t colorM[3];
    uint8_t colorS[3];

    uint16_t pollInterval;

    const bool operator==(const Configuration& other) {
        // FIXME: extend with correct comparison
        return (this->brightness == other.brightness &&
                this->showMinutes == other.showMinutes);
    }
};

Configuration defaultConfiguration = {
    127, true, true, true, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, 3 * 60};