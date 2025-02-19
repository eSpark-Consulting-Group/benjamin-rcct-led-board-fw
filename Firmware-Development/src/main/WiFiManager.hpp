#pragma once

#include "WiFi.h"
#include "BLEManager.hpp"
#include "MQTTManager.hpp"

#include "JSONManager.hpp"





class WiFiManager {
  public:
    static void attemptWiFiConnection();
    static bool credrecievedFlag( bool write , bool write_val ,bool read );
    static String networkScanner();


  private:
    static bool ssidExists(String ssid, String list);
};


