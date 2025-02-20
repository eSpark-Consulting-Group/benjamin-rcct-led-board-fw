#include "WiFiManager.hpp"

String wifiSsid, wifiPassword;
String scannedSsids = ""; // Define static variable

bool credentialsReceived=false;



String WiFiManager::scannedSsidsString(){
          return(scannedSsids);
    }

bool WiFiManager::credrecievedFlag( bool write , bool write_val ,bool read ){
    if(write == true){
        credentialsReceived= write_val;
    }
    else if(read == true){
      return(credentialsReceived);
    }
    return(credentialsReceived) ;

}

void WiFiManager::networkScanner(){
    Serial.println("Scanning available networks...");
    Serial.println("** Scan Networks **");
    int numSsid = WiFi.scanNetworks();
    if (numSsid == -1) {
        Serial.println("Couldn't get a wifi connection");
        while (true);
    }

    // print the list of networks seen:
    Serial.print("number of available networks:");
    Serial.println(numSsid);

    scannedSsids= "{ \"networks\": [";
    // print the network number and name for each network found:

        bool first = true;

          for (int i = 0; i < numSsid; i++) {
              String ssid = WiFi.SSID(i);
              if (! WiFiManager::ssidExists(ssid, scannedSsids)) { // Only add unique SSIDs

                  if (!first) scannedSsids += ", ";
                  // scannedSsids += "{ \"SSID\": \"" + ssid + "\", \"RSSI\": " + String(WiFi.RSSI(i)) + "\",
                  //                     \"Encryption Type\": " + String(WiFiManager::printEncryptionTypeEx(WiFi.encryptionTypeEx(i))) + " }";
                  scannedSsids += "{ \"SSID\": \"" + ssid + "\", \"RSSI\": " + String(WiFi.RSSI(i)) + " }";
                  first = false;
              }
          }

        scannedSsids += "] }"; // Close JSON array and object

}

bool WiFiManager::ssidExists(String ssid, String list) {
    return list.indexOf("\"" + ssid + "\"") != -1; // Check if SSID already exists
}





void WiFiManager::attemptWiFiConnection() {
      
    FlashManager::load_wifi_creds(wifiSsid , wifiPassword);

    Serial.println("Connecting to Wi-Fi...");
    
    char ssid_buf[wifiSsid.length() + 1];
    char pass_buf[wifiPassword.length() + 1];

    strcpy(ssid_buf, wifiSsid.c_str());
    strcpy(pass_buf, wifiPassword.c_str());

    WiFi.begin(ssid_buf, pass_buf);
    // wifiSsid = wifiPassword = "";

    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 5000; // 15 seconds timeout

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {

        vTaskDelay(pdMS_TO_TICKS(500)); 
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {

        BLEManager::updateDeviceStatus();
        Serial.println("\nWi-Fi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        credentialsReceived = false;

    } else {
        MQTTManager::disconnect();       
        BLEManager::updateDeviceStatus();   
        Serial.println("\nFailed to connect to Wi-Fi.");
    }
}

String WiFiManager::printEncryptionTypeEx(uint32_t thisType) {
    /*  Arduino wifi api use encryption type to mapping to security type.
    *  This function demonstrate how to get more richful information of security type.
    */
    switch (thisType) {
        case SECURITY_OPEN:
            return("Open");
            break;
        case SECURITY_WEP_PSK:
            return("WEP");
            break;
        case SECURITY_WPA_TKIP_PSK:
            return("WPA TKIP");
            break;
        case SECURITY_WPA_AES_PSK:
            return("WPA AES");
            break;
        case SECURITY_WPA2_AES_PSK:
            return("WPA2 AES");
            break;
        case SECURITY_WPA2_TKIP_PSK:
            return("WPA2 TKIP");
            break;
        case SECURITY_WPA2_MIXED_PSK:
            return("WPA2 Mixed");
            break;
        case SECURITY_WPA_WPA2_MIXED:
            return("WPA/WPA2 AES");
            break;
        case SECURITY_WPA3_AES_PSK:
            return("WPA3 AES");
            break;
        case SECURITY_WPA2_WPA3_MIXED:
            return("WPA2/WPA3");
    }
}
