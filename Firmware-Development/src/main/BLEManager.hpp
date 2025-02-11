#pragma once


#include "BLEDevice.h"
#include "WiFiManager.hpp"
#include "MQTTManager.hpp"
#include <Arduino_JSON.h>
#include "globflags.hpp"

#define WIFI_SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define WIFI_CREDENTIALS_CHAR_UUID "d752c5fb-1380-4cd5-b0ef-cac7d72cff20"
#define DEVICE_STATUS_CHAR_UUID    "e23a3e8c-1d8b-4b5a-88c9-fc3e2d5cbb23"


class BLEManager {
  public:
      // BLEManager();
      void init();
      void start();
      void updateDeviceStatus();
      static void onCredentialsWrite(BLECharacteristic* chr, uint8_t connID);

  // private:
  //     BLEService wifiService;
  //     BLECharacteristic wifiCredentialsChar;
  //     BLECharacteristic deviceStatusChar;
  };


