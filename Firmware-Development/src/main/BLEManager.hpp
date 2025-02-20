#pragma once

// #undef CHAR_VALUE_MAX_LEN
// #define CHAR_VALUE_MAX_LEN 500
#include "BLEDevice.h"
#include "BLECharacteristic.h"
#include "WiFiManager.hpp"
#include "MQTTManager.hpp"
#include "JSONManager.hpp"

// #define CHAR_VALUE_MAX_LEN 500


#define WIFI_SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define WIFI_CREDENTIALS_CHAR_UUID "d752c5fb-1380-4cd5-b0ef-cac7d72cff20"
#define DEVICE_STATUS_CHAR_UUID    "e23a3e8c-1d8b-4b5a-88c9-fc3e2d5cbb23"
#define AVAILABLE_SSID_CHAR_UUID    "fa5942ef-6dcc-42e6-b6f7-0c5368f68583"


class BLEManager {
  public:
      static void init();
      static void start();
      static void updateDeviceStatus();
      // static void  fetchSSIDS() ;
      static void ble_write_callback(BLECharacteristic* chr, uint8_t connID);
      static void ble_read_callback(BLECharacteristic* chr, uint8_t connID);
      static  char* getmacStr();
 
  };


