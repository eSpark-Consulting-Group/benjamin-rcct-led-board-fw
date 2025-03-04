#pragma once

#include <Arduino.h>

#include "config.hpp"
#include "LED.hpp"


// #include "FlashManager.hpp"
// #include "pwmout_api.h"

class AnimationManager{
  public:
    static void runSunriseAnimation();


  private:
    static void setSunriseAnimationValues(float warm_value, float cool_value, float red_value); 

};

