#include "HTTPManager.hpp"



const char server1[] = "api.ipgeolocation.io";
const char server1_path[] = "/timezone?apiKey=1f70bad104504ec483f32a86e13aec18";




WiFiClient client_new;


void HTTPManager::httpGetLocationData() {
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

        if (err == 200){
            String response;
            while (http1.available()) {
                  char c = http1.read();
                  response += c;


              }
            String time = JSONManager::parse_JSON(response , "time_24");
            Serial.println("Response while loop:" +response);

        }

}





