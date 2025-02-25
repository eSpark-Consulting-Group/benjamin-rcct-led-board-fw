#pragma once

#include <Arduino.h>
#include "pwmout_api.h"
#include "config.hpp"


typedef struct  {
    String color = DEFAULT_COLOR;
    uint16_t temperature = DEFAULT_TEMP;
    uint16_t brightness= DEFAULT_BRIGHTNESS;
 
}Config ;

typedef struct {
    int red =0;
    int green=0;
    int blue=0;
}rgb;


class SingleInput {
  public:

    static SingleInput& getInstance() {
        static SingleInput instance;
        return instance;
    }

    Config config;  // Global struct instance
    rgb RGB;
    pwmout_t pwm_red;
    pwmout_t pwm_cool_white;
    pwmout_t pwm_warm_white;

    static void PWM_init();
    static void config_bright_color_temp();
    static bool  single_input_recieved_flag( bool write , bool write_val ,bool read);




  private:
    SingleInput() {}  // Private constructor (singleton)
    static void hexToRGB(const String& hex, int& red, int& green, int& blue) ;
    static void config_brightness();
    static void config_temperature();
    static void config_color();

  
};


