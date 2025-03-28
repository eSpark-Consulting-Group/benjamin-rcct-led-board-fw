#include "WiFiManager.hpp"

String WiFiManager::wifiSsid = "";
String WiFiManager::wifiPassword = "";

bool credentialsReceived = false;

void WiFiManager::init()
{

    connect();
}

void WiFiManager::setCredentialsReceived(bool value)
{
    credentialsReceived = value;
}

bool WiFiManager::getCredentialsReceived()
{
    return credentialsReceived;
}

void WiFiManager::networkScanner()
{
    BLEManager::setCommandRecieved(false);

    Serial.println("Scanning available networks...");
    Serial.println("** Scan Networks **");
    int numSsid = WiFi.scanNetworks();
    if (numSsid == -1)
    {
        Serial.println("Couldn't get a wifi connection");
        while (true)
            ;
    }
    JSONManager::makeSSIDJSON(numSsid);
    // print the list of networks seen:
    Serial.print("number of available networks:");
    Serial.println(numSsid);
    BLEManager::setNotifySSID(true);
}

bool WiFiManager::ssidExists(String ssid, String list)
{
    return list.indexOf("\"" + ssid + "\"") != -1; // Check if SSID already exists
}

void WiFiManager::connect()
{

    FlashManager::load_wifi_creds(wifiSsid, wifiPassword);

    Serial.println("Connecting to Wi-Fi...");

    char ssid_buf[wifiSsid.length() + 1];
    char pass_buf[wifiPassword.length() + 1];

    strcpy(ssid_buf, wifiSsid.c_str());
    strcpy(pass_buf, wifiPassword.c_str());

    Serial.println("pass");
    WiFi.begin(ssid_buf, pass_buf);

    unsigned long startAttemptTime = millis();
    const unsigned long timeout = WIFI_TIMEOUT_MS; // 5 seconds timeout

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout)
    {

        Serial.print(".");
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    if (WiFi.status() == WL_CONNECTED)
    {

        BLEManager::updateDeviceStatus();
        Serial.println("\nWi-Fi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        credentialsReceived = false;
    }
    else
    {
        MQTTManager::disconnect();
        BLEManager::updateDeviceStatus();
        Serial.println("\nFailed to connect to Wi-Fi.");
    }
}

struct EncryptionType
{
    uint32_t type;
    String name;
};

String WiFiManager::printEncryptionTypeEx(uint32_t thisType)
{
    static const EncryptionType encryptionTypes[] = {
        {SECURITY_OPEN, "Open"},
        {SECURITY_WEP_PSK, "WEP"},
        {SECURITY_WPA_TKIP_PSK, "WPA TKIP"},
        {SECURITY_WPA_AES_PSK, "WPA AES"},
        {SECURITY_WPA2_AES_PSK, "WPA2 AES"},
        {SECURITY_WPA2_TKIP_PSK, "WPA2 TKIP"},
        {SECURITY_WPA2_MIXED_PSK, "WPA2 Mixed"},
        {SECURITY_WPA_WPA2_MIXED, "WPA/WPA2 AES"},
        {SECURITY_WPA3_AES_PSK, "WPA3 AES"},
        {SECURITY_WPA2_WPA3_MIXED, "WPA2/WPA3"}};
    for (size_t i = 0; i < sizeof(encryptionTypes) / sizeof(encryptionTypes[0]); ++i)
    {
        if (encryptionTypes[i].type == thisType)
        {
            return encryptionTypes[i].name;
        }
    }
    return "Unknown"; // Handle unexpected encryption types
}