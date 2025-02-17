#include <Arduino.h>
#include "FreeRTOS.h"
#include "task.h"
#include "BLEManager.hpp"
#include "WiFiManager.hpp"
#include "MQTTManager.hpp"
#include "single_input.hpp"
// #include "config.hpp"





void WiFiTask(void* pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(2000));
    while (1) {

        if ((WiFiManager::credrecievedFlag(false ,false ,true) == true) || WiFi.status() != WL_CONNECTED) {
            WiFiManager::attemptWiFiConnection() ;
        }

        if (WiFi.status() == WL_CONNECTED && !MQTTManager::isConnected()) {
            MQTTManager::attemptConnection();
        }

        if ((single_input_pwm::single_input_recieved_flag(false,false,true) == true)) {
            single_input_pwm::config_bright_color_temp();
        }

              
        MQTTManager::publish_JSON();
        MQTTManager::loop();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



void animation_task(void *pvParameters) {
    while(1){ 
      // single_input_pwm::run_sunrise();
      vTaskDelay(pdMS_TO_TICKS(2000));  // Check every 5 seconds           
    }

}

void setup() {
    Serial.begin(115200);
    FlashMemory.begin(FLASH_MEMORY_APP_BASE, 0x1000);
    FlashMemory.read();

    
    BLEManager::init() ;
    MQTTManager::init();
    single_input_pwm::PWM_init();

    xTaskCreate(WiFiTask, "WiFiTask", 4096, NULL, 2, NULL);
    xTaskCreate(animation_task, "animation_task", 4096, NULL, 1, NULL);

}

void loop() {}
