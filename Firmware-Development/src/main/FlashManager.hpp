#pragma once

#include <FlashMemory.h>
#include <Arduino.h>

#include "config.hpp"

class FlashManager
{
public:
  static void init();
  static void save_wifi_creds(String ssid, String pass);
  static void load_wifi_creds(String &ssid, String &pass);
  static void writeWord(uint32_t buffer, uint32_t address);
  static uint32_t readWord(uint32_t offsetAddress);
  static void verifyData(uint32_t offset, uint32_t elements);
};