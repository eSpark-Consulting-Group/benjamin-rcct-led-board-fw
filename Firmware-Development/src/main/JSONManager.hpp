#pragma once

#include <Arduino_JSON.h>
#include "WiFi.h"

#include "BLEManager.hpp"
#include "MQTTManager.hpp"
#include "WiFiManager.hpp"
#include "config.hpp"
// #include "JSONManager2.hpp"
#include "SingleInput.hpp"
#include "FlashManager.hpp"


class JSONManager{
      public:
        static int total_chunks_ssid_JSON();
        static String* chunks_ssid_JSON();
        static void make_ssid_JSON(int n);
        static void parse_JSON(String data);
        static String make_ble_JSON();
        static String make_mqtt_JSON();
        static void parse_ble_JSON(String ble_data);        
        static void parse_mqtt_JSON(String mqtt_data);
      
};