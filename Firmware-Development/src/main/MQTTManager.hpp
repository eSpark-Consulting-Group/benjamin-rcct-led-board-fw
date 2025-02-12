#pragma once

#include <PubSubClient.h>
#include "WiFi.h"
#include <Arduino_JSON.h>
#include "BLEManager.hpp"


class MQTTManager {
public:
    static void init();
    static void attemptConnection();
    static bool isConnected();
    static void callback(char* topic, byte* payload, unsigned int length);
    static void loop();
    static void disconnect();
    static void publish_JSON( bool Power_state , uint8_t brightness , String colour_code , String effect , uint16_t speed);

};


