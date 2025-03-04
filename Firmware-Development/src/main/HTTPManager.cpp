#include "HTTPManager.hpp"



const char server1[] = SERVER1_DOMAIN_NAME;
const char server1_path[] = SERVER1_CONFIG_PATH;

const char server2[] = SERVER2_DOMAIN_NAME;
const char server2_path[] = SERVER2_PATH;
uint16_t server2_port = SERVER2_PORT;

String responsejsonString ="";
String response2jsonString ="";


WiFiClient client_new;

void  HTTPManager::httpGetServer2Response() {
  HttpClient http2(client_new);

   int err = http2.get(server2, server2_port ,server2_path);
        if (err == 0) {
            Serial.println("startedRequest ok");

            err = http2.responseStatusCode();
            if (err >= 0) {
                Serial.print("Got status code: ");
                Serial.println(err);
            }
            else{
              Serial.println("Request failed with status code: " + String(err));           
            }
        }
        if (err == 400){
            String response;
            while (http2.available()) {
                  char c = http2.read();
                  response += c;


              }
            //Extract JSON from Response
            response2jsonString = JSONManager::jsonExtractFromString(response); 
            
            Serial.println("Response while loop:"+ response2jsonString);
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
        if (err == SERVER1_SUCCESS_CODE){
            String response;
            while (http1.available()) {
                  char c = http1.read();
                  response += c;


              }
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



