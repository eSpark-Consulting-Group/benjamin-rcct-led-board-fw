#pragma once

#include <Arduino.h>

#include "config.hpp"
#include "SingleInput.hpp"

// #include "FlashManager.hpp"
// #include "pwmout_api.h"

class sunrise_anim{
  public:
    static void run_sunrise();
    static void set_led_values(float warm_value, float cool_value, float red_value); 

};

