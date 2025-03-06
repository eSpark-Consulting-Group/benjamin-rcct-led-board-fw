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
    static void publish_JSON();
    static bool profileNameRecivedFlag( bool write , bool write_val ,bool read );
    static bool  ledConfigRecievedFlag( bool write , bool write_val ,bool read);

};


