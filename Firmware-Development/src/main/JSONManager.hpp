#pragma once

#include <Arduino_JSON.h>
#include <FlashMemory.h>
#include "WiFi.h"

#include "BLEManager.hpp"
#include "MQTTManager.hpp"
#include "WiFiManager.hpp"



class JSONManager{
      public:
        static String devicestatus_json_construction();
        static void parse_ble_input(String ble_data);





};