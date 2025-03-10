#include "HTTPManager.hpp"



const char server1[] = SERVER1_DOMAIN_NAME;
const char server1_path[] = SERVER1_CONFIG_PATH;

const char server2[] = SERVER2_DOMAIN_NAME;
const char server2_path[] = SERVER2_PATH;
uint16_t server2_port = SERVER2_PORT;

String responsejsonString ="";
String response2jsonString ="";


WiFiClient client_new;

void HTTPManager::httpGetServer2Response() {
      HttpClient http2(client_new);

      int err = http2.get(server2, server2_port, server2_path);
      if (err == 0) {
          Serial.println("startedRequest ok");

          err = http2.responseStatusCode();
          if (err >= 0) {
              Serial.print("Got status code: ");
              Serial.println(err);
          } else {
              Serial.println("Request failed with status code: " + String(err));           
              return;
          }
      }

      int byteCount = 0;
      int32_t tempWord = 0;  // Temporary 4-byte buffer
      int wordByteIndex = 0;  // Tracks how many bytes are in tempWord
      uint32_t flashAddress = 0x0100; // Start writing at 0x0100

      if (err == SERVER2_SUCCESS_CODE) {
              // Skip HTTP headers
              while (http2.available()) {
                  String line = http2.readStringUntil('\n');  // Read line-by-line
                  if (line == "\r") {  // Headers end with an empty line (\r\n\r\n)
                      break;
                  }
              }
              int byteCount = 0;
              Serial.println("Binary Data:");
              
              // Read binary data
              while (http2.available()) {
                uint8_t value = http2.read();  // Read raw binary byte-by-byte
                
                if (byteCount >= 12) { // Skip first 12 bytes 
                    Serial.print(value,DEC);
                    Serial.print(",");
        
                    // Collect 4 bytes into a 32-bit word
                    tempWord |= (value << (wordByteIndex * 8)); 
                    wordByteIndex++;
        
                    // When 4 bytes are collected, write to flash
                    if (wordByteIndex == 4) {
                        FlashMemory.writeWord(flashAddress, tempWord);

                        flashAddress += 4;  // Move to next flash word
                        tempWord = 0;       // Reset temp buffer
                        wordByteIndex = 0;  // Reset byte counter
                    }
                }
        
                byteCount++;
            }
            Serial.println("\nData read complete.");
            Serial.println("\nVerifying Flash Memory:");
            Serial.println(byteCount);

            uint32_t flashAddress = 0x0100; // Start reading from 0x0100
            for (int i = 0; i < (byteCount-12) / 4; i++) {  // Divide byteCount by 4 to get the number of words
                uint32_t storedValue = FlashMemory.readWord(flashAddress);
                Serial.print(storedValue);
                Serial.print(",");
                
                flashAddress += 4;  // Move to the next word
            }
      }

        MQTTManager::profileNameRecivedFlag(true ,false ,false);     
}

void HTTPManager::httpGetAPIResponse() {
  HttpClient http1(client_new);

   int err = http1.get(server1, server1_path);
        if (err == 0) {
            Serial.println("startedRequest ok");

            err = http1.responseStatusCode();
            if (err >= 0) {
                Serial.print("Got status code: ");
                Serial.println(err);
            }
            else{
              Serial.println("Request failed with status code: " + String(err));           
            }
        }

        if (err == SERVER2_SUCCESS_CODE) {
          // Skip HTTP headers
          while (http1.available()) {
              String line = http1.readStringUntil('\n');  // Read line-by-line
              if (line == "\r") {  // Headers end with an empty line (\r\n\r\n)
                  break;
              }
          }
        }

        if (err == SERVER1_SUCCESS_CODE){
            String response;
            while (http1.available()) {
                  char c = http1.read();
                  response += c;


              }
              Serial.println("[response] =" + response);
            //Extract JSON from Response
            responsejsonString = JSONManager::jsonExtractFromString(response);
            Serial.println("Response while loop:"+ responsejsonString);
        }
}


void HTTPManager::extractTimeFromAPIResponse() {
      String time = JSONManager::parse_JSON(responsejsonString , SERVER1_TIME_PROPERTY);  
      Serial.println("time:"+ time);

      String hoursStr = time.substring(0, 2);    //for HH:MM:SS   format
      String minutesStr = time.substring(3, 5);
      String secondsStr = time.substring(6, 8);
      
      RTCManager::getInstance().crntimeinfo.hour = (uint8_t)atoi(hoursStr.c_str());
      RTCManager::getInstance().crntimeinfo.min = (uint8_t)atoi(minutesStr.c_str());
      RTCManager::getInstance().crntimeinfo.sec = (uint8_t)atoi(secondsStr.c_str());

      Serial.print("Hours: ");
      Serial.println(RTCManager::getInstance().crntimeinfo.hour);
      Serial.print("Minutes: ");
      Serial.println(RTCManager::getInstance().crntimeinfo.min);
      Serial.print("Seconds: ");
      Serial.println(RTCManager::getInstance().crntimeinfo.sec);

}

void HTTPManager::extractDateFromAPIResponse(){

      String date = JSONManager::parse_JSON(responsejsonString , SERVER1_DATE_PROPERTY);
      String yearStr = date.substring(0, 4);          //for YYYY-MM-DD  foramt
      String monthStr = date.substring(5, 7);
      String dayStr = date.substring(8, 10);


      Serial.println(yearStr);
      Serial.println(monthStr);
      Serial.println(dayStr);

      RTCManager::getInstance().crntimeinfo.year = (uint16_t)atoi(yearStr.c_str());
      RTCManager::getInstance().crntimeinfo.month = (uint8_t)atoi(monthStr.c_str());
      RTCManager::getInstance().crntimeinfo.day = (uint8_t)atoi(dayStr.c_str());


      Serial.print("Year: ");
      Serial.println(RTCManager::getInstance().crntimeinfo.year);
      Serial.print("Month: ");
      Serial.println(RTCManager::getInstance().crntimeinfo.month);
      Serial.print("Day: ");
      Serial.println(RTCManager::getInstance().crntimeinfo.day);

}


void HTTPManager::extractDateAndTimeFromAPIResponse() {
      extractTimeFromAPIResponse();
      extractDateFromAPIResponse();

}


void HTTPManager::getResponse(const char* serverName, uint16_t serverPort , const char* path ) {
    HttpClient http2(client_new);

    int err = http2.get(server2, server2_port, server2_path);
    if (err == 0) {
        Serial.println("startedRequest ok");

        err = http2.responseStatusCode();
        if (err >= 0) {
            Serial.print("Got status code: ");
            Serial.println(err);
        } else {
            Serial.println("Request failed with status code: " + String(err));           
            return;
        }
    }

    if (err == SERVER2_SUCCESS_CODE) {
        // Skip HTTP headers
        while (http2.available()) {
            String line = http2.readStringUntil('\n');  // Read line-by-line
            if (line == "\r") {  // Headers end with an empty line (\r\n\r\n)
                break;
            }
        }
        int byteCount = 0;
        Serial.println("Binary Data:");



        if (err == SERVER1_SUCCESS_CODE){
            String response;
            while (http1.available()) {
                  char c = http1.read();
                  response += c;


              }
              Serial.println("[response] =" + response);

            while (http2.available()) {
                uint8_t value = http2.read();  // Read raw binary byte-by-byte










}


