#include "WiFiManager.hpp"

String wifiSsid, wifiPassword;

bool credentialsReceived=false;

void WiFiManager::loadWiFiCredentials() {
    Serial.println("Loading Wi-Fi credentials from flash...");

    char ssidBuf[64];
    char passBuf[64];

    strcpy(ssidBuf, (char *)(FlashMemory.buf + FLASH_SSID_OFFSET));
    strcpy(passBuf, (char *)(FlashMemory.buf + FLASH_PASS_OFFSET));

    if (strlen(ssidBuf) > 0 && strlen(passBuf) > 0) {
        wifiSsid = String(ssidBuf);
        wifiPassword = String(passBuf);
    }
}

bool WiFiManager::credrecievedFlag( bool write , bool write_val ,bool read ){
    if(write == true){
        credentialsReceived= write_val;
    }
    else if(read == true){
      return(credentialsReceived);
    }

}


void WiFiManager::attemptWiFiConnection() {
    
    Serial.println("Connecting to Wi-Fi...");
    
    char ssid_buf[wifiSsid.length() + 1];
    char pass_buf[wifiPassword.length() + 1];

    strcpy(ssid_buf, wifiSsid.c_str());
    strcpy(pass_buf, wifiPassword.c_str());

    WiFi.begin(ssid_buf, pass_buf);
    // wifiSsid = wifiPassword = "";

    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 5000; // 15 seconds timeout

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {

        vTaskDelay(pdMS_TO_TICKS(500)); 
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        BLEManager::updateDeviceStatus();
        Serial.println("\nWi-Fi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        credentialsReceived = false;

    } else {
        MQTTManager::disconnect();       
        BLEManager::updateDeviceStatus();   
        Serial.println("\nFailed to connect to Wi-Fi.");
    }
}


