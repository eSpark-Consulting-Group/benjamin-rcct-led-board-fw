#include "BLEDevice.h"
#include "WiFi.h"
#include "FreeRTOS.h"
#include "task.h"
#include <FlashMemory.h>


#define FLASH_SSID_OFFSET 0x0000
#define FLASH_PASS_OFFSET 0x0100
#define FLASH_MAX_LEN 64  // Adjust according to max SSID/password length

// Define the UUIDs for the custom service and characteristic
#define WIFI_SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define WIFI_CREDENTIALS_CHAR_UUID "d752c5fb-1380-4cd5-b0ef-cac7d72cff20"

#define STRING_BUF_SIZE 180

// Create the BLE service and characteristic
BLEService WifiService(WIFI_SERVICE_UUID);
BLECharacteristic WifiCredentialsChar(WIFI_CREDENTIALS_CHAR_UUID);



BLEAdvertData advdata;
BLEAdvertData scndata;

// Variables to store Wi-Fi credentials
String wifiSsid, wifiPassword;
bool credentialsReceived = false;


// Function to parse JSON and extract SSID and Password
void parseWiFiCredentials(const String& jsonData) {
    int ssidStart = jsonData.indexOf("\"ssid\"") + 7;  // Move to after "ssid"
    ssidStart = jsonData.indexOf("\"", ssidStart) + 1; // Find opening quote
    int ssidEnd = jsonData.indexOf("\"", ssidStart);   // Find closing quote

    int passStart = jsonData.indexOf("\"password\"") + 11; // Move to after "password"
    passStart = jsonData.indexOf("\"", passStart) + 1;     // Find opening quote
    int passEnd = jsonData.indexOf("\"", passStart);       // Find closing quote

    if (ssidStart != -1 && ssidEnd != -1 && passStart != -1 && passEnd != -1) {
        wifiSsid = jsonData.substring(ssidStart, ssidEnd);
        wifiPassword = jsonData.substring(passStart, passEnd);
        

        Serial.print("Parsed SSID: ");
        Serial.println(wifiSsid);
        Serial.print("Parsed Password: ");
        Serial.println(wifiPassword);
        saveWiFiCredentials();

    } else {
        Serial.println("Failed to parse JSON!");
    }
}

// Callback for when Wi-Fi credentials are written
void onCredentialsWrite(BLECharacteristic* chr, uint8_t connID) {
    credentialsReceived = true;
    Serial.print("Wi-Fi Credentials written by connection ");
    Serial.println(connID);

    if (chr->getDataLen() > 0) {
        String jsonData = chr->readString();
        jsonData.trim();
        Serial.print("Received JSON: ");
        Serial.println(jsonData);

        parseWiFiCredentials(jsonData);

    }
}

void saveWiFiCredentials() {
    Serial.println("Saving Wi-Fi credentials to flash...");

    // Ensure strings fit in buffer
    if (wifiSsid.length() >= FLASH_MAX_LEN || wifiPassword.length() >= FLASH_MAX_LEN) {
        Serial.println("Error: SSID or Password too long!");
        return;
    }

    memset(FlashMemory.buf, 0, FLASH_MAX_LEN * 2); // Clear previous data
    strcpy((char *)(FlashMemory.buf + FLASH_SSID_OFFSET), wifiSsid.c_str());
    strcpy((char *)(FlashMemory.buf + FLASH_PASS_OFFSET), wifiPassword.c_str());

    FlashMemory.update();
    Serial.println("Wi-Fi credentials saved.");
}

void loadWiFiCredentials() {
    Serial.println("Loading Wi-Fi credentials from flash...");

    char ssidBuf[FLASH_MAX_LEN];
    char passBuf[FLASH_MAX_LEN];

    strcpy(ssidBuf, (char *)(FlashMemory.buf + FLASH_SSID_OFFSET));
    strcpy(passBuf, (char *)(FlashMemory.buf + FLASH_PASS_OFFSET));

    if (strlen(ssidBuf) > 0 && strlen(passBuf) > 0) {
        wifiSsid = String(ssidBuf);
        wifiPassword = String(passBuf);

        Serial.print("Loaded SSID: ");
        Serial.println(wifiSsid);
        Serial.print("Loaded Password: ");
        Serial.println(wifiPassword);

        // Attempt connection
        // attemptWiFiConnection();
    } else {
        Serial.println("No saved Wi-Fi credentials found.");
    }
}


// Function to attempt Wi-Fi connection (Restored to Your Original Version)
void attemptWiFiConnection() {

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

void WiFiTask(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(2000));  // Check every 5 seconds           
    while (1) {
        if (credentialsReceived || WiFi.status() != WL_CONNECTED) {
            attemptWiFiConnection();
        }        
        printCurrentNet();
        vTaskDelay(pdMS_TO_TICKS(1000));  // Check every 5 seconds           

    }
}

void setup() {
    Serial.begin(115200);

        // Initialize Flash Memory
    FlashMemory.begin(FLASH_MEMORY_APP_BASE, 0x1000);
    FlashMemory.read();
    loadWiFiCredentials();


    // Load saved Wi-Fi credentials
    
    // Configure advertisement data
    advdata.addFlags();
    advdata.addCompleteName("RCCT_LED_BRD");
    scndata.addCompleteServices(BLEUUID(WIFI_SERVICE_UUID));

    // Configure Wi-Fi Credentials characteristic
    WifiCredentialsChar.setWriteProperty(true);
    WifiCredentialsChar.setWritePermissions(GATT_PERM_WRITE);
    WifiCredentialsChar.setWriteCallback(onCredentialsWrite);
    WifiCredentialsChar.setBufferLen(STRING_BUF_SIZE);

    // Add characteristic to the service
    WifiService.addCharacteristic(WifiCredentialsChar);

    // Initialize BLE
    BLE.init();
    BLE.configAdvert()->setAdvData(advdata);
    BLE.configAdvert()->setScanRspData(scndata);
    BLE.configServer(1);
    BLE.addService(WifiService);

    // Start BLE peripheral
    BLE.beginPeripheral();
    xTaskCreate(WiFiTask, "WiFiTask", 4096, NULL, 1, NULL);

    Serial.println("BLE Peripheral started. Waiting for Wi-Fi credentials...");
}

void printCurrentNet() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print the MAC address of the router you're attached to:
    byte bssid[6];
    WiFi.BSSID(bssid);
    Serial.print("BSSID: ");
    Serial.print(bssid[5], HEX);
    Serial.print(":");
    Serial.print(bssid[4], HEX);
    Serial.print(":");
    Serial.print(bssid[3], HEX);
    Serial.print(":");
    Serial.print(bssid[2], HEX);
    Serial.print(":");
    Serial.print(bssid[1], HEX);
    Serial.print(":");
    Serial.println(bssid[0], HEX);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.println(rssi);

    // print the encryption type:
    byte encryption = WiFi.encryptionType();
    Serial.print("Encryption Type:");
    Serial.println(encryption, HEX);
    Serial.println();
}

void loop() {

}