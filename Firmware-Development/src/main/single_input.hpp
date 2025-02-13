#pragma once

#include <Arduino.h>
#include "pwmout_api.h"
#include "config.hpp"


class single_input_pwm {
public:
    static void PWM_init();
    static void run_sunrise();
    static void set_led_values(float warm_value, float cool_value, float red_value);

private:
    static pwmout_t pwm_red;
    static pwmout_t pwm_cool_white;
    static pwmout_t pwm_warm_white;
};