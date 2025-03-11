#pragma once

#include <Arduino.h>

#include "config.hpp"
#include "LED.hpp"
#include "HTTPManager.hpp"
#include "FlashManager.hpp"

class ProfileManager{
  public:
    static void runSunriseProfile();
    static void runProfile();


  private:
    static void setLEDPWM(float coolWhiteDuty, float redDuty, float warmWhiteDuty) ;
    static uint32_t timeDiff( uint32_t offsetAddress);


};

