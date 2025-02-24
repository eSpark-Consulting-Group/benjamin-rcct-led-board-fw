#include "JSONManager.hpp"

String chunks[50];
int totalChunksSsidJson = 0;


String JSONManager::parse_JSON(String inputJSON , String property){
            String propertyValue;
            JSONVar data_JSON = JSON.parse(inputJSON);
            if (JSON.typeof(data_JSON) == "undefined") {
                Serial.println("Failed to parse JSON!");
                return("Failed to parse JSON!");
            }
            Serial.println("Parsed JSON:");
            Serial.println(JSON.stringify(data_JSON));
            
              if (data_JSON.hasOwnProperty(property)){
                   propertyValue = (const char*)data_JSON[property];
                   return(propertyValue);
              }

}



String* JSONManager::chunks_ssid_JSON(){
  return(chunks);

}

int JSONManager::total_chunks_ssid_JSON(){
      return(totalChunksSsidJson);
}


String JSONManager::make_JSON(String param , String state){
         JSONVar jsonParam;
         jsonParam[param] = state;
          String jsonString = JSON.stringify(jsonParam) ;
        return(jsonString);
}
  

void JSONManager::make_ssid_JSON(int n){
        JSONVar wifiList;
       std::set<String> seenSSIDs;  // To track unique SSIDs

        for (int i = 0; i < n; i++) {
          JSONVar network;
          String ssid = WiFi.SSID(i);
          
          if (seenSSIDs.find(ssid) != seenSSIDs.end()) {
            continue;  // Skip duplicates
          }
          seenSSIDs.insert(ssid);

          network["SSID"] = WiFi.SSID(i);
          network["RSSI"] = WiFi.RSSI(i);
          network["ENCRYPTION"] = WiFiManager::printEncryptionTypeEx(WiFi.encryptionTypeEx(i));
           wifiList[seenSSIDs.size() - 1] = network;
        }
        String jsonString = JSON.stringify(wifiList);
        Serial.print("created JSON:");
        Serial.println(jsonString);

        for (size_t i = 0; i < jsonString.length() && totalChunksSsidJson < 50; i += 200) {
            chunks[totalChunksSsidJson] = jsonString.substring(i, i + 200);
            Serial.print("CHUNK DATA:");
            Serial.println(chunks[totalChunksSsidJson]);
            totalChunksSsidJson++;
          }
        
}




String JSONManager::make_ble_JSON(){

    JSONVar deviceStatus;
    deviceStatus["RCCT MAC ADDRESS "] = BLEManager::getmacStr();
    deviceStatus["RCCT WiFi STATUS"] = WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected";
    deviceStatus["WiFi SSID"] = WiFi.SSID();
    deviceStatus["MQTT SERVER"] = MQTTManager::isConnected() ?  "Connected" : "Disconnected";

    String jsonString = JSON.stringify(deviceStatus);
    return(jsonString);
 
}

String JSONManager::make_mqtt_JSON(){
      JSONVar mqtt_JSON;
      mqtt_JSON["Power"] = POWER;
      mqtt_JSON["Brightness"] = BRIGHTNESS;
      mqtt_JSON["color"] = COLOR;
      mqtt_JSON["effect"] = EFFECT;
      mqtt_JSON["speed"] = SPEED;

    
      return (JSON.stringify(mqtt_JSON));

}




void JSONManager::parse_ble_JSON(String ble_data){

        String parsed_ssid,parsed_password;
        ble_data.trim();
        Serial.print("Received BLE JSON: ");
        Serial.println(ble_data);
        JSONVar creds = JSON.parse(ble_data);

            if (creds.hasOwnProperty("ssid") && creds.hasOwnProperty("password")) {     //for WIFI CREDS
                  WiFiManager::credrecievedFlag( true, true ,false );
                  Serial.print("Wi-Fi Credentials written by connection ");
                  parsed_ssid = (const char*)creds["ssid"];
                  parsed_password = (const char*)creds["password"];

                  Serial.print("Parsed SSID: ");
                  Serial.println(parsed_ssid);

                  if (parsed_ssid.length() >= 64 || parsed_password.length() >= 64) {
                  Serial.println("Error: SSID or Password too long!");
                  return;}

                  FlashManager::save_wifi_creds(parsed_ssid , parsed_password );

              }


}

void JSONManager::parse_mqtt_JSON(String mqtt_data){
            JSONVar mqtt_JSON = JSON.parse(mqtt_data);
            if (JSON.typeof(mqtt_JSON) == "undefined") {
                Serial.println("Failed to parse JSON!");
                return;
            }
            Serial.println("Parsed MQTT JSON:");
            Serial.println(JSON.stringify(mqtt_JSON));


                if (mqtt_JSON.hasOwnProperty("color") && mqtt_JSON.hasOwnProperty("temperature") && mqtt_JSON.hasOwnProperty("brightness")) {
                    SingleInput::single_input_recieved_flag( true , true ,false);
                    SingleInput::getInstance().config.color = (const char *)mqtt_JSON["color"];
                    SingleInput::getInstance().config.temperature = (uint16_t)mqtt_JSON["temperature"];
                    SingleInput::getInstance().config.brightness = (uint16_t)mqtt_JSON["brightness"];
 
                    Serial.println("Parsed Values:");
                    Serial.println("Color: " + SingleInput::getInstance().config.color);
                    Serial.println("Temperature: " + String(SingleInput::getInstance().config.temperature));
                    Serial.println("Brightness: " + String(SingleInput::getInstance().config.brightness));
                    
                    SingleInput::config_bright_color_temp();


              }


}







