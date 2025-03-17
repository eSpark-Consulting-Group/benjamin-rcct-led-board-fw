#include "BLEManager.hpp"


bool commandRecieved= false;
bool  notifySSID= false;
bool invalidCommand =false;
char macStr[MAC_STRING_SIZE];
uint8_t connectionID;
int currentChunk = 0;  // To track which chunk to send

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
    WifiCredentialsChar.setBufferLen(WIFICREDSCHAR_BUFFER_LEN);     //180

    DeviceStatusChar.setReadProperty(true);
    DeviceStatusChar.setReadPermissions(GATT_PERM_READ);
    DeviceStatusChar.setBufferLen(DEVICESTATUSCHAR_BUFFER_LEN);      //180


    AvailableSsidChar.setNotifyProperty(true);  // Allow writing
    AvailableSsidChar.setWriteProperty(true);  // Allow writing
    AvailableSsidChar.setWritePermissions(GATT_PERM_WRITE);
    AvailableSsidChar.setWriteCallback(scan_callback);
    AvailableSsidChar.setBufferLen(AVAILABLESSIDCHAR_BUFFER_LEN);

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

    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    const int numProps = 4;

    String keys[numProps] = { "RCCT MAC ADDRESS", "RCCT WiFi STATUS", "WiFi SSID","MQTT SERVER" };

    String values[numProps] = { 
        BLEManager::getmacStr(), 
        WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected" ,
        WiFi.SSID(),
        MQTTManager::isConnected() ?  "Connected" : "Disconnected"
    };


    SubJSON subJSONs[] = {
        { "device status", numProps, keys, values }
    };

    String jsonOutput = JSONManager::makeJSON(NULL, NULL, NULL , 1 , subJSONs);
     
    DeviceStatusChar.writeString(jsonOutput.c_str());
    Serial.println("[updateDeviceStatus] jsonOutput: " + jsonOutput); 
}



void BLEManager::ble_write_callback(BLECharacteristic* chr, uint8_t connID) {
    Serial.println(connID);

    if (chr->getDataLen() > 0) {
        // JSONManager::parse_ble_JSON(chr->readString()); //BLE DATA PARSE
        String parsed_ssid = JSONManager::parseJSON(chr->readString(), "ssid") ;
        String parsed_password =  JSONManager::parseJSON(chr->readString(), "password") ;
        if((parsed_ssid.startsWith("Invalid property")) || (parsed_password.startsWith("Invalid property"))){
            Serial.println("Invalid Command");
            return;
        }
        else{
            WiFiManager::credrecievedFlag( true, true ,false );
            Serial.print("Wi-Fi Credentials written by connection ");
            Serial.print("Parsed SSID: ");
            Serial.println(parsed_ssid);

            if (parsed_ssid.length() >= SSID_BUFF_LEN || parsed_password.length() >= PASS_BUFF_LEN) {
            Serial.println("Error: SSID or Password too long!");
            return;}        
            FlashManager::save_wifi_creds(parsed_ssid , parsed_password );
        }
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

bool BLEManager::commandRecievedFlag( bool write , bool write_val ,bool read ){
    if(write == true){
        commandRecieved= write_val;
    }
    else if(read == true){
      return(commandRecieved);
    }
    return(commandRecieved) ;

}

bool BLEManager::invalidCommandFlag( bool write , bool write_val ,bool read ){
    if(write == true){
        invalidCommand= write_val;
    }
    else if(read == true){
      return(invalidCommand);
    }
    return(invalidCommand) ;
}

void BLEManager::scan_callback(BLECharacteristic* chr, uint8_t connID){
      connectionID=connID;

      if (chr->getDataLen() > 0) {

        String command = JSONManager::parseJSON(chr->readString() , "cmd"); 

        if( command == CMD_SCAN_START){
          commandRecieved= true;}

          else{
            invalidCommand = true;
          }

      }
}

void BLEManager::sendInvalidCommandNotifications(){
       String error;

            const int numProps = 1;
            String keys[numProps] = { "status"};       
            String values[numProps] = {STATUS_INVALID_COMMAND};       
            error = JSONManager::makeJSON(numProps, keys, values);
            
            const char* strData = error.c_str();
            uint16_t length = error.length();
            AvailableSsidChar.setData((uint8_t*)strData, length);   
            AvailableSsidChar.notify(connectionID);
            Serial.println(STATUS_INVALID_COMMAND); 
           invalidCommand = false;
}


void BLEManager::sendSSIDJSONNotifications(){
    String endingMessage ;
    const int numProps = 1;

    while(currentChunk < JSONManager::gettotalSSIDChunks()) {

          String dataToSend = (JSONManager::getSSIDChunkNumber())[currentChunk];  //return the pointer for chunk array defined in JSONmanager and access its memeber and saves in datastring
          Serial.print("total chunks: ");
          Serial.println(JSONManager::gettotalSSIDChunks());

          Serial.print("dataToSend: ");
          Serial.println(dataToSend);
 
            const char* strData = dataToSend.c_str();
            uint16_t length = dataToSend.length();

            if (AvailableSsidChar.setData((uint8_t*)strData, length)) {
                AvailableSsidChar.notify(connectionID);

                Serial.println("Notification sent successfully.");
            } else {
                Serial.println("Failed to set data.");
                String keys[numProps] = { "status"};       
                String values[numProps] = {STATUS_FAILED_SCAN};       
                endingMessage = JSONManager::makeJSON(numProps, keys, values);

            }

          currentChunk++;  // Move to the next chunk      
      } 
      
      if (currentChunk == JSONManager::gettotalSSIDChunks()){
          String keys[numProps] = { "status"};       
          String values[numProps] = {STATUS_SUCCESSFUL_SCAN};       
          endingMessage = JSONManager::makeJSON(numProps, keys, values);
          
      }

      const char* strData = endingMessage.c_str();
      uint16_t length = endingMessage.length();
      AvailableSsidChar.setData((uint8_t*)strData, length);   
      AvailableSsidChar.notify(connectionID);

      currentChunk = 0;  // Reset for the next full transmission    
      notifySSID= false;


}



