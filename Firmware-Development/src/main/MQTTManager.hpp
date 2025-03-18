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
    static bool profileNameRecivedFlag( bool write , bool write_val ,bool read );
    static bool ledConfigRecievedFlag( bool write , bool write_val ,bool read);
    static bool getOtaUpdateAvailable();
    static void setOtaUpdateAvailable(bool state);
    static void handleControlTopics(const String &message);
    static void handleProfileTopic(const String &message);
    static void handleOtaTopic(const String &message);
    static void handleOtaUpdate(const String &message);
    static void handleChunkTopic2Sub(const String &message);
    static void handleChunkTopic3Sub(const String &message);
    

};


