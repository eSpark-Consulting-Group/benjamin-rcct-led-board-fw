#pragma once

#include "BLEDevice.h"
#include "BLECharacteristic.h"

#include "WiFiManager.hpp"
#include "MQTTManager.hpp"
#include "JSONManager.hpp"
#include "config.hpp"

class BLEManager
{

public:
  static void init();
  static void ble_write_callback(BLECharacteristic *chr, uint8_t connID);
  static void scan_callback(BLECharacteristic *chr, uint8_t connID);
  static char *getMac();
  static void setCommandRecieved(bool value);
  static bool getCommandRecieved();
  static void setNotifySSID(bool value);
  static bool getNotifySSID();
  static void setInvalidCommand(bool value);
  static bool getInvalidCommand();
  static void sendInvalidCommandNotifications();
  static void sendSSIDJSONNotifications();
  static void updateDeviceStatus();
};
