#pragma once
#include <stdint.h>

struct Configuration
{
    uint8_t brightness;

    bool show_minutes;
    bool show_seconds;

    uint8_t colorH[3];
    // irrelevant if "!showX"
    uint8_t colorM[3];
    uint8_t colorS[3];

    uint16_t poll_interval_min;
/*
    bool operator==(const Configuration &s2)
    {
        return (this->brightness == s2.brightness);
    }*/
};

Configuration default_configuration = {
    127, 
    true, 
    true, 
    {255, 0, 0}, 
    {255, 0, 0}, 
    {255, 0, 0}, 
    3*60
};