#include <Arduino.h>
#include "FreeRTOS.h"
#include "task.h"
#include "BLEManager.hpp"
#include "WiFiManager.hpp"
#include "MQTTManager.hpp"
#include "LED.hpp"
#include "AnimationManager.hpp"
#include "RTCManager.hpp"
#include "HTTPManager.hpp"
#include "OTAManager.hpp"
// #include "config.hpp"


TaskHandle_t taskAnimationExecuteHandle = NULL;
TaskHandle_t taskNetworkScannerHandle = NULL;


void taskMajor(void* pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(2000));    

    while (1) {           
        if ((WiFiManager::credrecievedFlag(false ,false ,true) == true || WiFi.status() != WL_CONNECTED)) {
                 WiFiManager::attemptWiFiConnection();

        }

        if (WiFi.status() == WL_CONNECTED && !MQTTManager::isConnected()) {
            HTTPManager::httpGetAPIResponse() ;
            HTTPManager::extractDateAndTimeFromAPIResponse();
            RTCManager::syncRTCDateAndTime();
            RTCManager::setExecutionTime();
            MQTTManager::attemptConnection();
        }

        if ((LED::single_input_recieved_flag(false,false,true) == true)) {
            LED::config_bright_color_temp();
        }

        if((BLEManager::commandRecievedFlag(false ,false ,true)) == true){
            xTaskCreate(taskNetworkScanner, "taskNetworkScanner", 2048, NULL, 1, &taskNetworkScannerHandle);        
        }

        if((BLEManager::notifySSIDFlag(false ,false ,true)) == true){
                BLEManager::sendSSIDJSONNotifications();
        }
        if(BLEManager::invalidCommandFlag(false ,false ,true) ==true){
                BLEManager::sendInvalidCommandNotifications();
        }

        if(RTCManager::executionTimeCheck() == true){
            xTaskCreate(taskAnimationExecute, "taskAnimationExecute", 2048, NULL, 1, &taskAnimationExecuteHandle);        
        }

        if(MQTTManager::profileNameRecivedFlag(false ,false ,true) ==true){
            HTTPManager::httpGetServer2Response() ;            
        }

        RTCManager::printDateAndTime();       
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}


void taskMinor(void *pvParameters) {   
    while(1){ 
      MQTTManager::loop();
      // MQTTManager::publish_JSON();
      vTaskDelay(pdMS_TO_TICKS(10));  // Check every 5 seconds           
    }

}

void taskNetworkScanner(void *pvParameters) {
    WiFiManager::networkScanner();
    vTaskDelete(NULL);  
}

void taskAnimationExecute(void *pvParameters) {
    AnimationManager::runSunriseAnimation();
    vTaskDelete(NULL);  
}

void setup() {
    Serial.begin(115200);
    FlashMemory.begin(FLASH_MEMORY_APP_BASE, 0x1000);
    FlashMemory.read();


    
    BLEManager::init();
    RTCManager::init();
    MQTTManager::init();
    OTAManager::init();
    LED::PWM_init();

    xTaskCreate(taskMajor, "taskMajor", 4096, NULL, 2, NULL);
    xTaskCreate(taskMinor, "taskMinor", 2048, NULL, 1, NULL);

}

void loop() {}
