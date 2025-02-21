#include "BLEManager.hpp"

bool scanCommandRecieved= false;
bool  notifySSID= false;
char macStr[18];
uint8_t connectionID;
int currentChunk = 0;  // To track which chunk to send




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

   AvailableSsidChar.setNotifyProperty(true);  // Allow writing
    AvailableSsidChar.setWriteProperty(true);  // Allow writing
    AvailableSsidChar.setWritePermissions(GATT_PERM_WRITE);
    AvailableSsidChar.setWriteCallback(scan_callback);
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



void BLEManager::ble_write_callback(BLECharacteristic* chr, uint8_t connID) {
    Serial.println(connID);

    if (chr->getDataLen() > 0) {
        JSONManager::parse_ble_JSON(chr->readString()); //BLE DATA PARSE


    }
}



void BLEManager::ble_read_callback(BLECharacteristic* chr, uint8_t connID) {
 
    if (currentChunk == -1){
      String chunkInfo = "SSID INFO CHUNKS :"+ String(JSONManager::total_chunks_ssid_JSON());
      AvailableSsidChar.writeString(chunkInfo.c_str());
      currentChunk = 0;  // Start sending chunks from the next read
    }

    else if (currentChunk < JSONManager::total_chunks_ssid_JSON()) {

      String dataToSend = (JSONManager::chunks_ssid_JSON())[currentChunk];  //return the pointer for chunk array defined in JSONmanager and access its memeber and saves in datastring
      Serial.print("total chunks: ");
      Serial.println(JSONManager::total_chunks_ssid_JSON());

      Serial.print("dataToSend: ");
      Serial.println(dataToSend);
      AvailableSsidChar.writeString(dataToSend.c_str());

      currentChunk++;  // Move to the next chunk
    } else {
      AvailableSsidChar.writeString("SSID INFO CHUNKS ENDED");
      currentChunk = -1;  // Reset for the next full transmission
    }

}

bool BLEManager::notifySSIDFlag( bool write , bool write_val ,bool read ){
    if(write == true){
        notifySSID= write_val;
    }
    else if(read == true){
      return(notifySSID);
    }
    return(notifySSID) ;

}

bool BLEManager::scanCommandRecievedFlag( bool write , bool write_val ,bool read ){
    if(write == true){
        scanCommandRecieved= write_val;
    }
    else if(read == true){
      return(scanCommandRecieved);
    }
    return(scanCommandRecieved) ;

}

void BLEManager::scan_callback(BLECharacteristic* chr, uint8_t connID){
      scanCommandRecieved= true;
      connectionID=connID;
}

void BLEManager::sendSSIDJSONNotifications(){

    while(currentChunk < JSONManager::total_chunks_ssid_JSON()) {

          String dataToSend = (JSONManager::chunks_ssid_JSON())[currentChunk];  //return the pointer for chunk array defined in JSONmanager and access its memeber and saves in datastring
          Serial.print("total chunks: ");
          Serial.println(JSONManager::total_chunks_ssid_JSON());

          Serial.print("dataToSend: ");
          Serial.println(dataToSend);
          // AvailableSsidChar.writeString(dataToSend.c_str());
          AvailableSsidChar.setData(6);

          AvailableSsidChar.notify(connectionID);

          currentChunk++;  // Move to the next chunk
          vTaskDelay(pdMS_TO_TICKS(400));  // Check every 5 seconds           
      }    
      // AvailableSsidChar.notify("SSID INFO CHUNKS ENDED");
      currentChunk = 0;  // Reset for the next full transmission
    
      notifySSID= false;


}




