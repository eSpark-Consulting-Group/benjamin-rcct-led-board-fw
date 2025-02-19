#include "JSONManager.hpp"




String JSONManager::make_ble_JSON(){

    // return create(
    //     std::make_pair("RCCT MAC ADDRESS", BLEManager::getmacStr()),
    //     std::make_pair("RCCT WiFi STATUS", WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected"),
    //     std::make_pair("WiFi SSID", WiFi.SSID()),
    //     std::make_pair("MQTT SERVER", MQTTManager::isConnected() ? "Connected" : "Disconnected")
    // );
    JSONVar deviceStatus;
    deviceStatus["RCCT MAC ADDRESS "] = BLEManager::getmacStr();
    deviceStatus["RCCT WiFi STATUS"] = WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected";
    deviceStatus["WiFi SSID"] = WiFi.SSID();
    // deviceStatus["BLE DEVICE"] = BLE.connected() ? "Connected" : "Disconnected";
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

                    // FlashManager::save_single_input_vals(SingleInput::getInstance().config.color , 
                    //                                     SingleInput::getInstance().config.temperature, 
                    //                                       SingleInput::getInstance().config.brightness);

              }


}


// void JSONManager::parse_http_JSON(String mqtt_data)




