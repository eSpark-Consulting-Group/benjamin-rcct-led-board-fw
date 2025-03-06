#pragma once


#include <FlashMemory.h>
#include <Arduino.h>

#include "config.hpp"





class FlashManager{
  public:
    static void save_wifi_creds(String ssid , String pass);
    static void load_wifi_creds(String &ssid , String &pass);





};