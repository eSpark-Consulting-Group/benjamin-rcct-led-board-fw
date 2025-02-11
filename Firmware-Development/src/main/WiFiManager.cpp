#include "WiFiManager.hpp"

String wifiSsid, wifiPassword;


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

void WiFiManager::saveWiFiCredentials() {
    if (wifiSsid.length() >= 64 || wifiPassword.length() >= 64) {
        Serial.println("Error: SSID or Password too long!");
        return;
    }

    memset(FlashMemory.buf, 0, 128);
    strcpy((char *)(FlashMemory.buf + FLASH_SSID_OFFSET), wifiSsid.c_str());
    strcpy((char *)(FlashMemory.buf + FLASH_PASS_OFFSET), wifiPassword.c_str());

    FlashMemory.update();
    Serial.println("Wi-Fi credentials saved.");
}

void WiFiManager::parseWiFiCredentials(const String& jsonData) {
    JSONVar json = JSON.parse(jsonData);

    if (json.hasOwnProperty("ssid") && json.hasOwnProperty("password")) {
        wifiSsid = (const char*)json["ssid"];
        wifiPassword = (const char*)json["password"];

        Serial.print("Parsed SSID: ");
        Serial.println(wifiSsid);
        saveWiFiCredentials();
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
    const unsigned long timeout = 10000; // 15 seconds timeout

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
        vTaskDelay(pdMS_TO_TICKS(500)); 
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWi-Fi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());

        credentialsReceived = false;

    } else {
        Serial.println("\nFailed to connect to Wi-Fi.");
    }
}
