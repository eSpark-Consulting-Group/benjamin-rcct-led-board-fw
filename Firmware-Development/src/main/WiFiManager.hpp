#pragma once

#include "WiFi.h"
#include <FlashMemory.h>
#include "BLEManager.hpp"
#include "MQTTManager.hpp"



#define FLASH_SSID_OFFSET 0x0000
#define FLASH_PASS_OFFSET 0x0100
#define FLASH_MAX_LEN 64  // Adjust according to max SSID/password length



class WiFiManager {
public:
    static void loadWiFiCredentials();
    static void attemptWiFiConnection();
    static bool credrecievedFlag( bool write , bool write_val ,bool read );

};


