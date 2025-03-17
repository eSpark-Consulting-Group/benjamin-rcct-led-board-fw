#include "JSONManager.hpp"

String ssidChunkNumber[SSID_JSON_MAX_CHUNKS];
int totalSSIDChunks = 0;



String JSONManager::parseJSON(String inputJSON, String property) {
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
              // Recursively call parseJSON on the nested object
              return parseJSON(JSON.stringify(data_JSON[firstKey]), remainingPath);
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

String* JSONManager::getSSIDChunkNumber(){
      return(ssidChunkNumber);

} 

int JSONManager::gettotalSSIDChunks(){
      return(totalSSIDChunks);
}

  

void JSONManager::makeSSIDJSON(int n){
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

        for (size_t i = 0; i < jsonString.length() && totalSSIDChunks < SSID_JSON_MAX_CHUNKS; i += SSID_JSON_MAX_SIZE) {
          ssidChunkNumber[totalSSIDChunks] = jsonString.substring(i, i + SSID_JSON_MAX_SIZE);
            Serial.print("CHUNK DATA:");
            Serial.println(ssidChunkNumber[totalSSIDChunks]);
            totalSSIDChunks++;
          }
        
}

 String JSONManager::makeJSON(int numProps, const String *keys, const String *values, 
  int numSubJSONs , const SubJSON *subJSONs ){

      JSONVar jsonData;

      // Add top-level key-value pairs
      for (int i = 0; i < numProps; i++) {
        jsonData[keys[i]] = values[i];
      }

      // Add sub-JSON objects if provided
      for (int i = 0; i < numSubJSONs; i++) {
        addSubJSON(jsonData, subJSONs[i].key, subJSONs[i].numProps, subJSONs[i].keys, subJSONs[i].values);
      }

      return JSON.stringify(jsonData);
}

void JSONManager::addSubJSON(JSONVar& parent, const String& key, int numProps, 
        const String *keys, const String *values) {

      JSONVar subJson;

      for (int i = 0; i < numProps; i++) {
      subJson[keys[i]] = values[i];
      }
      parent[key] = subJson;
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




