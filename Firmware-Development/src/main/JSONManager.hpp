#pragma once

#include <Arduino_JSON.h>
#include "WiFi.h"
#include <set>


#include "BLEManager.hpp"
#include "MQTTManager.hpp"
#include "WiFiManager.hpp"
#include "LED.hpp"
#include "FlashManager.hpp"
#include "config.hpp"


class JSONManager{
      public:
        static int total_chunks_ssid_JSON();
        static String* chunks_ssid_JSON();
        static String make_JSON(String param , String state);
        static void make_ssid_JSON(int n);
        static String make_ble_JSON();
        static String make_mqtt_JSON();
        static String parse_JSON(String inputJSON , String property);
        static void parse_ble_JSON(String ble_data);        
        static String jsonExtractFromString(String &response);


      private:
        static String convertToString(JSONVar value); 
};