#pragma once

#include "WiFi.h"

#include "BLEManager.hpp"
#include "MQTTManager.hpp"
#include "JSONManager.hpp"
#include "config.hpp"




class WiFiManager {
  public:
    static void init();
    static void connect();
    // static bool wifiCredsCheck();
    // static bool credrecievedFlag( bool write , bool write_val ,bool read );
    static void setCredentialsReceived(bool value);
    static bool getCredentialsReceived();
    static void networkScanner(); 
    static String printEncryptionTypeEx(uint32_t thisType);
    
  private:
    static bool ssidExists(String ssid, String list);
    static String wifiSsid;
    static String wifiPassword;
};


