#pragma once

#include <Arduino.h>
#include "pwmout_api.h"
#include "config.hpp"

#include "MQTTManager.hpp"

typedef struct
{
    bool power;
    String effect;

    struct
    {
        String colorString;
        int coolWhite;
        int red;
        int warmWhite;
    } color;

    String profileName;
    uint16_t brightness;
    uint16_t speed;

    struct
    {
        String version;
        String url;
    } ota;

    uint16_t temperature;

} LEDConfig;

class LED
{
public:
    static LED &getInstance()
    {
        static LED instance;
        return instance;
    }

    LEDConfig ledconfig;

    pwmout_t pwmCoolWhite;
    pwmout_t pwmRed;
    pwmout_t pwmWarmWhite;

    static void init();
    static void configureLED();

private:
    LED() {} // Private constructor (singleton)
    static void hexToCRW(const String &hex, int &red, int &green, int &blue);
    static void configBrightness();
    static void configColor();
};
