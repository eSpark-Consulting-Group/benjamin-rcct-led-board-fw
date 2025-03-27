#include "HTTPManager.hpp"

int byteCount = 0;

String responsejsonString ="";

WiFiClient client_new;


void HTTPManager::getRealTime() {
    HttpClient http(client_new);
    ServerInfo serverInf;

    serverInf.serverName = SERVER1_DOMAIN_NAME;
    serverInf.path = SERVER1_CONFIG_PATH;
    serverInf.serverPort = SERVER1_PORT;
    serverInf.successCode = SERVER1_SUCCESS_CODE;

    if(getStatusCode(&serverInf ,&http) !=   serverInf.successCode){       //get status code
        printf("[get Response] Status Code: %d\n\r", serverInf.successCode);
        return ;
    };
    printf("[get Response] Status Code: %d\n\r", serverInf.successCode);
    String response;
    readHttpHeaders(&http);   
    getResponseString(response , &http);
    responsejsonString = JSONManager::jsonExtractFromString(response);
    Serial.println("[getRealTime] Extracted JSON:" + responsejsonString);
    

  }

void HTTPManager::extractTimeFromAPIResponse() {
      String time = JSONManager::parseJSON(responsejsonString , SERVER1_TIME_PROPERTY);  
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

      String date = JSONManager::parseJSON(responsejsonString , SERVER1_DATE_PROPERTY);
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

int HTTPManager::getByteCount(){
    return(byteCount);
}

void HTTPManager::getProfile() {
    HttpClient http(client_new);
    ServerInfo serverInf;

    serverInf.serverName = SERVER2_DOMAIN_NAME;
    serverInf.path = SERVER2_PATH;
    serverInf.serverPort = SERVER2_PORT;
    serverInf.successCode = SERVER2_SUCCESS_CODE;

    if(getStatusCode(&serverInf ,&http) !=   serverInf.successCode){       //get status code
        printf("[get Response] Status Code: %d\n\r", serverInf.successCode);
        return ;
    };
    printf("[get Response] Status Code: %d\n\r", serverInf.successCode);

    readHttpHeaders(&http);                                               //skip http headers

    byteCount = 0;
    uint32_t tempWord = 0;  // Temporary 4-byte buffer
    int wordByteIndex = 0;  // Tracks how many bytes are in tempWord
    uint32_t flashAddress = PROFILE_START_ADDR; // Start writing at PROFILE_START_ADDR



    while (http.available()) {
        uint8_t value = http.read();  // Read raw binary byte-by-byte
        
        if (byteCount >= 12) { // Skip first 12 bytes 
            Serial.print(value,DEC);
            Serial.print(",");

            // Collect 4 bytes into a 32-bit word
            tempWord |= (value << (wordByteIndex * 8)); 
            wordByteIndex++;

            // When 4 bytes are collected, write to flash
            if (wordByteIndex == WORD_SIZE) {
                FlashManager::writeWord( tempWord, flashAddress);

                flashAddress += WORD_SIZE;  // Move to next flash word
                tempWord = 0;       // Reset temp buffer
                wordByteIndex = 0;  // Reset byte counter
            }
        }

        byteCount++;
      }
        Serial.println("\nData read complete.");
        Serial.println("\nVerifying Flash Memory:");
        Serial.println(byteCount);
        FlashManager::verifyData(PROFILE_START_ADDR ,(byteCount-12) / WORD_SIZE);
     
    // MQTTManager::profileNameRecivedFlag(true ,false ,false);  
       MQTTManager::setProfileNameRecived(false);   
                

}


int16_t HTTPManager::getStatusCode(ServerInfo *serverInf ,HttpClient *http){

    int err = http->get(serverInf->serverName, serverInf->serverPort,  serverInf->path);
    if (err == 0) {
        Serial.println("startedRequest ok");

        err = http->responseStatusCode();
        if (err >= 0) {
            Serial.println("[getStatusCode] Status Code Success");
            return err;

        } else {
            Serial.println("[getStatusCode]Status Code Failed" );           
            return err;
        }
    }
}


void HTTPManager::readHttpHeaders(HttpClient *http){
        // Skip HTTP headers
        while (http->available()) {
            String line = http->readStringUntil('\n');  // Read line-by-line
            if (line == "\r") {  // Headers end with an empty line (\r\n\r\n)
                break;
            }
        }

}

void HTTPManager::getResponseString(String &response , HttpClient *http){   
    while (http->available()) {
          char c = http->read();
          response += c;


      }
      Serial.println("[response] =" + response);

}

