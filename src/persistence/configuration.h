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
                this->pollInterval == other.pollInterval);
    }
};

Configuration defaultConfiguration = {
    127, true, true, true, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, 3 * 60};