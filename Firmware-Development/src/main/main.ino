#include <Arduino.h>
#include "FreeRTOS.h"
#include "task.h"
#include "BLEManager.hpp"
#include "WiFiManager.hpp"
#include "MQTTManager.hpp"
#include "SingleInput.hpp"
#include "sunrise.hpp"
#include "RTCManager.hpp"
#include "HTTPManager.hpp"

// #include "config.hpp"




void WiFiTask(void* pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(2000));

    while (1) {

        if ((WiFiManager::credrecievedFlag(false ,false ,true) == true) || WiFi.status() != WL_CONNECTED) {
            WiFiManager::attemptWiFiConnection() ;

        }

        if (WiFi.status() == WL_CONNECTED && !MQTTManager::isConnected()) {
            HTTPManager::httpGetLocationData() ;
            MQTTManager::attemptConnection();
        }

        if ((SingleInput::single_input_recieved_flag(false,false,true) == true)) {
            SingleInput::config_bright_color_temp();
        }

        
        // MQTTManager::publish_JSON();
        MQTTManager::loop();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



void animation_task(void *pvParameters) {
    while(1){ 
      // sunrise_anim::run_sunrise();
      if((BLEManager::commandRecievedFlag(false ,false ,true)) == true){
            WiFiManager::networkScanner();
        }
      if((BLEManager::notifySSIDFlag(false ,false ,true)) == true){
            BLEManager::sendSSIDJSONNotifications();
      }
      if(BLEManager::invalidCommandFlag(false ,false ,true) ==true){
            BLEManager::sendInvalidCommandNotifications();
      }
      
      RTCManager::updateRTCTime();
      vTaskDelay(pdMS_TO_TICKS(100));  // Check every 5 seconds           
    }

}

void setup() {
    Serial.begin(115200);
    FlashMemory.begin(FLASH_MEMORY_APP_BASE, 0x1000);
    FlashMemory.read();

    
    BLEManager::init() ;
    RTCManager::init();
    RTCManager::enableAlarm();
    MQTTManager::init();
    SingleInput::PWM_init();

    xTaskCreate(WiFiTask, "WiFiTask", 4096, NULL, 2, NULL);
    xTaskCreate(animation_task, "animation_task", 4096, NULL, 1, NULL);

}

void loop() {}
