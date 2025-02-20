#pragma once

#include "WiFi.h"
#include "BLEManager.hpp"
#include "MQTTManager.hpp"

#include "JSONManager.hpp"





class WiFiManager {
  public:
    // static String scannedSsidsString();
    
    static String scannedSsidsString();
    static void attemptWiFiConnection();
    static bool credrecievedFlag( bool write , bool write_val ,bool read );
    static void networkScanner();

  private:
    static bool ssidExists(String ssid, String list);
    static String printEncryptionTypeEx(uint32_t thisType);


};


