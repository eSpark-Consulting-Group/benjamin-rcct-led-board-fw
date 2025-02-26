#pragma once


#include "BLEDevice.h"
#include "BLECharacteristic.h"

#include "WiFiManager.hpp"
#include "MQTTManager.hpp"
#include "JSONManager.hpp"
#include "config.hpp"





class BLEManager {
  public:
      static void init();
      static void start();
      static void updateDeviceStatus();
      static void ble_write_callback(BLECharacteristic* chr, uint8_t connID);
      static void scan_callback(BLECharacteristic* chr, uint8_t connID);
      static  char* getmacStr();
      static bool commandRecievedFlag( bool write , bool write_val ,bool read );
      static bool notifySSIDFlag( bool write , bool write_val ,bool read );
      static bool invalidCommandFlag( bool write , bool write_val ,bool read );
      static void sendInvalidCommandNotifications();
      static void sendSSIDJSONNotifications();
  };


