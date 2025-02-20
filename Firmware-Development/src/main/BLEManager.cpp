#include "BLEManager.hpp"


char macStr[18];





// wifi config service
BLEService WifiService(WIFI_SERVICE_UUID);
BLECharacteristic WifiCredentialsChar(WIFI_CREDENTIALS_CHAR_UUID);  // wifi creds input
BLECharacteristic DeviceStatusChar(DEVICE_STATUS_CHAR_UUID);        // device status output
BLECharacteristic AvailableSsidChar(AVAILABLE_SSID_CHAR_UUID);        // device status output



BLEAdvertData advdata;
BLEAdvertData scndata;


void BLEManager::init() {


    advdata.addFlags();
    advdata.addCompleteName("RCCT_LED_BRD");
    scndata.addCompleteServices(BLEUUID(WIFI_SERVICE_UUID));

    WifiCredentialsChar.setWriteProperty(true);
    WifiCredentialsChar.setWritePermissions(GATT_PERM_WRITE);
    WifiCredentialsChar.setWriteCallback(ble_write_callback);
    WifiCredentialsChar.setBufferLen(180);     //180

    DeviceStatusChar.setReadProperty(true);
    DeviceStatusChar.setReadPermissions(GATT_PERM_READ);
    DeviceStatusChar.setBufferLen(180);      //180

    AvailableSsidChar.setReadProperty(true);
    AvailableSsidChar.setReadPermissions(GATT_PERM_READ);
    AvailableSsidChar.setReadCallback(ble_read_callback);
    AvailableSsidChar.setBufferLen(230);

    WifiService.addCharacteristic(AvailableSsidChar);
    WifiService.addCharacteristic(DeviceStatusChar);
    WifiService.addCharacteristic(WifiCredentialsChar);

    BLE.init();
    BLE.configAdvert()->setAdvData(advdata);
    BLE.configAdvert()->setScanRspData(scndata);
    BLE.configServer(1);
    BLE.addService(WifiService);
    BLE.beginPeripheral();
}

char* BLEManager::getmacStr() {
    // char macStr[18] ;
    return macStr;
}

void BLEManager::updateDeviceStatus() {
    uint8_t mac[6];
    WiFi.macAddress(mac);

    // char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);


    String jsonString = JSONManager::make_ble_JSON();

    DeviceStatusChar.writeString(jsonString.c_str());
    Serial.print("RCCT Status Updated: ");
    Serial.println(jsonString);
}

// void  BLEManager::fetchSSIDS() {
//     String jsonAvailableSsid = WiFiManager::networkScanner();
//     AvailableSsidChar.writeString(jsonAvailableSsid.c_str());

    
//     Serial.print("Available Networks: ");
//     Serial.println(jsonAvailableSsid);

// }

void BLEManager::ble_write_callback(BLECharacteristic* chr, uint8_t connID) {
    Serial.println(connID);

    if (chr->getDataLen() > 0) {
        JSONManager::parse_ble_JSON(chr->readString()); //BLE DATA PARSE


    }
}

int chunkSize = 210;
uint32_t ssidReadPosition = 0;

void BLEManager::ble_read_callback(BLECharacteristic* chr, uint8_t connID) {
 

    String jsonAvailableSsid =  WiFiManager::scannedSsidsString();
    // Serial.println(jsonAvailableSsid);
    if (ssidReadPosition >= jsonAvailableSsid.length()) {
        ssidReadPosition = 0;  // Reset when done
        return;
    }

    // Ensure we don’t read past the end of the string
    uint32_t endPosition = ssidReadPosition + chunkSize;
    if (endPosition > jsonAvailableSsid.length()) {
        endPosition = jsonAvailableSsid.length();
    }

    // Extract the chunk
    String chunk = jsonAvailableSsid.substring(ssidReadPosition, endPosition);
    Serial.print("Sending chunk: ");
    Serial.println(chunk);
    Serial.print("ssidReadPosition: ");
    Serial.println(ssidReadPosition);
    Serial.print("endPosition: ");
    Serial.println(endPosition);
    // Indicate if more chunks are coming
    if (endPosition < jsonAvailableSsid.length()) {
        chunk += ",{\"more\":true}";
    } else {
        chunk += ",{\"more\":false}";
    }

    AvailableSsidChar.writeString(chunk.c_str());

    // pCharacteristic->setValue(chunk.c_str()); // Send chunk
    ssidReadPosition = endPosition; // Move to next chunk
    //  vTaskDelay(pdMS_TO_TICKS(10));
}




