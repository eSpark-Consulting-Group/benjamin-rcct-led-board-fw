#include "BLEDevice.h"
#include "WiFi.h"

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
bool printflag = true;

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
        credentialsReceived = true;

        Serial.print("Parsed SSID: ");
        Serial.println(wifiSsid);
        Serial.print("Parsed Password: ");
        Serial.println(wifiPassword);
    } else {
        Serial.println("Failed to parse JSON!");
    }
}

// Callback for when Wi-Fi credentials are written
void onCredentialsWrite(BLECharacteristic* chr, uint8_t connID) {
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

// Function to attempt Wi-Fi connection (Restored to Your Original Version)
void attemptWiFiConnection() {
    if (printflag) {
        Serial.println("Connecting to Wi-Fi...");
    }

    char ssid_buf[wifiSsid.length() + 1];
    char pass_buf[wifiPassword.length() + 1];

    strcpy(ssid_buf, wifiSsid.c_str());
    strcpy(pass_buf, wifiPassword.c_str());

    WiFi.begin(ssid_buf, pass_buf);
    wifiSsid = wifiPassword = "";

    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 15000; // 15 seconds timeout

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED && printflag) {
        Serial.println("\nWi-Fi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        printflag = false;
    } else if (printflag) {
        Serial.println("\nFailed to connect to Wi-Fi.");
        credentialsReceived = false;
    }
}

void setup() {
    Serial.begin(115200);

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
    if (credentialsReceived) {
        attemptWiFiConnection();
    }
    printCurrentNet();
    delay(1000);
}

