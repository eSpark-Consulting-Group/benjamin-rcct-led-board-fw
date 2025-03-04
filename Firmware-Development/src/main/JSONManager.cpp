#include "JSONManager.hpp"

String ssidChunks[SSID_JSON_MAX_CHUNKS];
int totalChunksSsidJson = 0;



String JSONManager::parse_JSON(String inputJSON, String property) {
  JSONVar data_JSON = JSON.parse(inputJSON);

      if (JSON.typeof(data_JSON) == "undefined") {
          Serial.println("Failed to parse JSON!");
          return "Failed to parse JSON!";
      }

      Serial.println("Parsed JSON:");
      Serial.println(JSON.stringify(data_JSON));

      // Split property path by dots for nested keys (e.g., "address.city")
      int dotIndex = property.indexOf(".");
      if (dotIndex == -1) {
          // If no dot, return the property directly
          if (data_JSON.hasOwnProperty(property)) {
              return convertToString(data_JSON[property]);
          }
      } else {
          // Handle nested properties
          String firstKey = property.substring(0, dotIndex);
          String remainingPath = property.substring(dotIndex + 1);

          if (data_JSON.hasOwnProperty(firstKey) && JSON.typeof(data_JSON[firstKey]) == "object") {
              // Recursively call parse_JSON on the nested object
              return parse_JSON(JSON.stringify(data_JSON[firstKey]), remainingPath);
          }
      }

      return "Invalid property";
}

String JSONManager::convertToString(JSONVar value) {
     String type = JSON.typeof(value);
  
      if (type == "string") {
          return (const char*)value;
      } else if (type == "number") {
          return String((double)value);
      } else if (type == "boolean") {
          return value ? "true" : "false";
      } else if (type == "array" || type == "object") {
          return JSON.stringify(value); // Return JSON as string
      }
      
      return "Unknown Type";
}

String* JSONManager::chunks_ssid_JSON(){
  return(ssidChunks);

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

        for (size_t i = 0; i < jsonString.length() && totalChunksSsidJson < SSID_JSON_MAX_CHUNKS; i += SSID_JSON_MAX_SIZE) {
          ssidChunks[totalChunksSsidJson] = jsonString.substring(i, i + SSID_JSON_MAX_SIZE);
            Serial.print("CHUNK DATA:");
            Serial.println(ssidChunks[totalChunksSsidJson]);
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



//Extract JSON from Response
String JSONManager::jsonExtractFromString(String &response){
              int braceCount = 0;
              int jsonStart = -1;
              String jsonString="";

              for (int i = 0; i < response.length(); i++) {
                char c = response.charAt(i);

                // Detect the start of JSON
                if (c == '{') {
                  if (jsonStart == -1) {
                    jsonStart = i;  // Mark the start of JSON
                  }
                  braceCount++;
                }

                // Add characters if inside JSON
                if (jsonStart != -1) {
                  jsonString += c;
                }

                // Detect the end of JSON when braces are balanced
                if (c == '}') {
                  braceCount--;
                  if (braceCount == 0) {
                    break;  // Complete JSON extracted
                       }
                  }
               }
                return(jsonString);
}




