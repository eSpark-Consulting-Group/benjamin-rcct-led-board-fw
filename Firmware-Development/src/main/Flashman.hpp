#pragma once


#include <FlashMemory.h>
#include <Arduino.h>

#include "config.hpp"





class flashman{
  public:
    static void save_wifi_creds(String ssid , String pass);
    static void load_wifi_creds(String &ssid , String &pass);
    static void save_single_input_vals(String col , uint16_t temp , uint16_t bright);
    static void load_single_input_vals(String &col, uint16_t &temp, uint16_t &bright) ;




};