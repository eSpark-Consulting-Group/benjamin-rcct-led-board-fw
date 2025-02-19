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
            Serial.println("Response while loop:" +response);

        }

}



    // if (err == 200) {
    //     Serial.println("Response :" + String(response));  // Print the response body

    //     JSONVar myObject = JSON.parse(response);
    //     Serial.println("parsed json:" + JSON.stringify(myObject));

    //     Serial.println("Request failed with status code: " + String(err));
    // }


  // Extract and print specific fields
  // String city = myObject["city"];
  // String country = myObject["country"];
  // float lat = myObject["lat"];
  // float lon = myObject["lon"];

  // Serial.print("City: ");
  // Serial.println(city);
  // Serial.print("Country: ");
  // Serial.println(country);
  // Serial.print("Latitude: ");
  // Serial.println(lat);
  // Serial.print("Longitude: ");
  // Serial.println(lon);

    
    
 



// // Extract latitude, longitude, and timezone
// float latitude = (double)doc["lat"];
// float longitude = (double)doc["lon"];
// String timezone = (const char*)doc["timezone"];

// Serial.println("Latitude: " + String(latitude));
// Serial.println("Longitude: " + String(longitude));
// Serial.println("Timezone: " + timezone);

