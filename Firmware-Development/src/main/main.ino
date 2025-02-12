#include "FreeRTOS.h"
#include "task.h"
#include "BLEManager.hpp"
#include "WiFiManager.hpp"
#include "MQTTManager.hpp"
#include "globflags.hpp"




BLEManager bleManager;

void WiFiTask(void* pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(2000));
    while (1) {
        if (credentialsReceived || WiFi.status() != WL_CONNECTED) {
            WiFiManager::attemptWiFiConnection();
        }
        if (WiFi.status() == WL_CONNECTED && !MQTTManager::isConnected()) {
            MQTTManager::attemptConnection();
        }
        // MQTTManager::publish_JSON();
        MQTTManager::publish_JSON( true , 300 , "#FFFFF" , "Solid" , 50);
        MQTTManager::loop();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// void check_staus(){
//       if(WiFi.status() == WL_CONNECTED){ bleManager.updateDeviceStatus(); }


// }


// void status_update_task(void *pvParameters) {
//     while(1){
//       bleManager.updateDeviceStatus();       
//       vTaskDelay(pdMS_TO_TICKS(2000));  // Check every 5 seconds           
//     }

// }

void setup() {
    Serial.begin(115200);
    FlashMemory.begin(FLASH_MEMORY_APP_BASE, 0x1000);
    FlashMemory.read();

    WiFiManager::loadWiFiCredentials();
    bleManager.init();
    MQTTManager::init();

    xTaskCreate(WiFiTask, "WiFiTask", 4096, NULL, 2, NULL);
    // xTaskCreate(status_update_task, "status_update_task", 4096, NULL, 1, NULL);

}

void loop() {}
