#pragma once

#include "WiFi.h"
#include "BLEManager.hpp"
#include "MQTTManager.hpp"





class WiFiManager {
public:
    static void attemptWiFiConnection();
    static bool credrecievedFlag( bool write , bool write_val ,bool read );

};


