#include "JSONManager.hpp"





String JSONManager::devicestatus_json_construction(){
    JSONVar deviceStatus;
    deviceStatus["RCCT MAC ADDRESS "] = BLEManager::getmacStr();
    deviceStatus["RCCT WiFi STATUS"] = WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected";
    deviceStatus["WiFi SSID"] = WiFi.SSID();
    // deviceStatus["BLE DEVICE"] = BLE.connected() ? "Connected" : "Disconnected";
    deviceStatus["MQTT SERVER"] = MQTTManager::isConnected() ?  "Connected" : "Disconnected";

    String jsonString = JSON.stringify(deviceStatus);
    return(jsonString);
 
}

void JSONManager::parse_ble_input(String ble_data){

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

              memset(FlashMemory.buf, 0, 128);
              strcpy((char *)(FlashMemory.buf + FLASH_SSID_OFFSET), parsed_ssid.c_str());
              strcpy((char *)(FlashMemory.buf + FLASH_PASS_OFFSET), parsed_password.c_str());

              FlashMemory.update();
              Serial.println("Wi-Fi credentials saved.");
          }


}