#pragma once

#include <stdint.h>

// no namespace, just like configuration.h

// include this first if you get some weird "type not defined" errors
struct Time {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;

    const bool operator==(const Time& other) {
        return (this->hour == other.hour && this->minute == other.minute &&
                this->second == other.second && this->millisecond == other.millisecond);
    }

    const bool operator>(const Time& other) {
        return (this->hour > other.hour || this->minute > other.minute ||
                this->second > other.second || this->millisecond > other.millisecond);
    }

        const bool operator<(const Time& other) {
        return (this->hour < other.hour && this->minute < other.minute &&
                this->second < other.second && this->millisecond < other.millisecond);
    }
};
