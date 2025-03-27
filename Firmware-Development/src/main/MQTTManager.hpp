#pragma once

#include <PubSubClient.h>
// #include "WiFi.h"

#include "BLEManager.hpp"
#include "JSONManager.hpp"
#include "config.hpp"

class MQTTManager {
public:
    static void init();
    static void attemptConnection();
    static bool isConnected();
    static void callback(char* topic, byte* payload, unsigned int length);
    static void loop();
    static void disconnect();
    static void publishLedStatePayload();
    // static bool profileNameRecivedFlag( bool write , bool write_val ,bool read );
    static void setProfileNameRecived(bool value);
    static bool getProfileNameRecived() ;
    // static bool  ledConfigRecievedFlag( bool write , bool write_val ,bool read);
    static void setLedConfigReceived(bool value);
    static bool getLedConfigReceived() ;
    static bool getOtaUpdateAvailable();
    static void setOtaUpdateAvailable(bool state);

};


