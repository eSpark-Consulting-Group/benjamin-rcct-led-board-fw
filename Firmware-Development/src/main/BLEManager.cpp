#include "BLEManager.hpp"




BLEService WifiService(WIFI_SERVICE_UUID);
BLECharacteristic WifiCredentialsChar(WIFI_CREDENTIALS_CHAR_UUID);  // wifi creds input
BLECharacteristic DeviceStatusChar(DEVICE_STATUS_CHAR_UUID);        // device status output

BLEAdvertData advdata;
BLEAdvertData scndata;


void BLEManager::init() {


    advdata.addFlags();
    advdata.addCompleteName("RCCT_LED_BRD");
    scndata.addCompleteServices(BLEUUID(WIFI_SERVICE_UUID));

    WifiCredentialsChar.setWriteProperty(true);
    WifiCredentialsChar.setWritePermissions(GATT_PERM_WRITE);
    WifiCredentialsChar.setWriteCallback(onCredentialsWrite);
    WifiCredentialsChar.setBufferLen(180);

    DeviceStatusChar.setReadProperty(true);
    DeviceStatusChar.setReadPermissions(GATT_PERM_READ);
    DeviceStatusChar.setBufferLen(180);

    WifiService.addCharacteristic(DeviceStatusChar);
    WifiService.addCharacteristic(WifiCredentialsChar);

    BLE.init();
    BLE.configAdvert()->setAdvData(advdata);
    BLE.configAdvert()->setScanRspData(scndata);
    BLE.configServer(1);
    BLE.addService(WifiService);
    BLE.beginPeripheral();
}

void BLEManager::updateDeviceStatus() {
    uint8_t mac[6];
    WiFi.macAddress(mac);

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    JSONVar deviceStatus;
    deviceStatus["RCCT MAC ADDRESS "] = macStr;
    deviceStatus["RCCT WiFi STATUS"] = WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected";
    deviceStatus["WiFi SSID"] = WiFi.SSID();
    deviceStatus["BLE DEVICE"] = BLE.connected() ? "Connected" : "Disconnected";
    deviceStatus["MQTT SERVER"] = MQTTManager::isConnected() ?  "Connected" : "Disconnected";

    String jsonString = JSON.stringify(deviceStatus);
    DeviceStatusChar.writeString(jsonString.c_str());

    Serial.print("Device Status Updated: ");
    Serial.println(jsonString);
}

void BLEManager::onCredentialsWrite(BLECharacteristic* chr, uint8_t connID) {
     credentialsReceived = true;
    Serial.print("Wi-Fi Credentials written by connection ");
    Serial.println(connID);

    if (chr->getDataLen() > 0) {
        String jsonData = chr->readString();
        jsonData.trim();
        Serial.print("Received JSON: ");
        Serial.println(jsonData);

        WiFiManager::parseWiFiCredentials(jsonData);
    }
}
