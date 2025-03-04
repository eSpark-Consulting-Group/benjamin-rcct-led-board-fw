#pragma once

#include <Arduino.h>
#include "pwmout_api.h"
#include "config.hpp"

   

typedef struct  {
    String power;
    String effect;

    struct {
        String colorString;
        int red;
        int green;
        int blue;
    } color;

    String profileName;
    uint16_t brightness;
    uint16_t speed;

    struct {
        String version;
        String url;
    }ota;
    
    uint16_t temperature;

}LEDConfig ;



class LED {
  public:

    static LED& getInstance() {
        static LED instance;
        return instance;
    }

    LEDConfig config;  // Global struct instance
    LEDConfig ledconfig;

    pwmout_t pwm_red;
    pwmout_t pwm_cool_white;
    pwmout_t pwm_warm_white;

    static void PWM_init();
    static void config_bright_color_temp();
    static bool  single_input_recieved_flag( bool write , bool write_val ,bool read);




  private:
    LED() {}  // Private constructor (singleton)
    static void hexToRGB(const String& hex, int& red, int& green, int& blue) ;
    static void config_brightness();
    static void config_temperature();
    static void config_color();

  
};


