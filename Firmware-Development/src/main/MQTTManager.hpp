#pragma once

#include <PubSubClient.h>
#include "WiFi.h"
#include <Arduino_JSON.h>

class MQTTManager {
public:
    static void init();
    static void attemptConnection();
    static bool isConnected();
    static void callback(char* topic, byte* payload, unsigned int length);
    static void loop();
};


