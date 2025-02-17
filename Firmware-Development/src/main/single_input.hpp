#pragma once

#include <Arduino.h>
#include "pwmout_api.h"
#include "config.hpp"
#include "Flashman.hpp"

typedef struct  {
    String color ="";
    uint16_t temperature =0;
    uint16_t brightness=0;
 
}Config ;

typedef struct {
    int red =0;
    int green=0;
    int blue=0;
}rgb;


class single_input_pwm {
  public:

    static single_input_pwm& getInstance() {
        static single_input_pwm instance;
        return instance;
    }

    Config config;  // Global struct instance
    rgb RGB;

    static void PWM_init();
    static void config_bright_color_temp();
    static bool  single_input_recieved_flag( bool write , bool write_val ,bool read);

    // static void run_sunrise();
    // static void set_led_values(float warm_value, float cool_value, float red_value);


  private:
    single_input_pwm() {}  // Private constructor (singleton)
    static void hexToRGB(const String& hex, int& red, int& green, int& blue) ;
    static void config_brightness();
    static void config_temperature();
    static void config_color();

    static pwmout_t pwm_red;
    static pwmout_t pwm_cool_white;
    static pwmout_t pwm_warm_white;

};


