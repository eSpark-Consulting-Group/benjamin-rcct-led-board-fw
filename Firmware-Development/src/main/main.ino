#include <Arduino.h>
#include "FreeRTOS.h"
#include "task.h"
#include "BLEManager.hpp"
#include "WiFiManager.hpp"
#include "MQTTManager.hpp"
#include "LED.hpp"
#include "ProfileManager.hpp"
#include "RTCManager.hpp"
#include "HTTPManager.hpp"
#include "OTAManager.hpp"
#include "config.hpp"

TaskHandle_t runProfileHandle = NULL;
TaskHandle_t networkScannerHandle = NULL;
TaskHandle_t OTAUpdateTaskHandle = NULL;

void mainTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(2000));

    while (1)
    {
        if ((WiFiManager::getCredentialsReceived() == true || WiFi.status() != WL_CONNECTED))
        {
            WiFiManager::connect();
        }

        if (WiFi.status() == WL_CONNECTED && !MQTTManager::isConnected())
        {

            HTTPManager::getRealTime();
            HTTPManager::extractDateAndTimeFromAPIResponse();
            RTCManager::syncRTCDateAndTime();
            RTCManager::setExecutionTime();
            MQTTManager::attemptConnection();
        }

        if ((MQTTManager::getOtaUpdateAvailable() == true))
        {
            xTaskCreate(OTAUpdateTask, "OTAUpdateTask", 2048, NULL, 1, &OTAUpdateTaskHandle);
        }

        if ((MQTTManager::getLedConfigReceived() == true))
        {
            Serial.println("LED Config Recieved getter");
            LED::configureLED();
        }

        if ((BLEManager::getCommandRecieved()) == true)
        {
            xTaskCreate(networkScanner, "networkScanner", 2048, NULL, 1, &networkScannerHandle);
        }

        if ((BLEManager::getNotifySSID()) == true)
        {
            BLEManager::sendSSIDJSONNotifications();
        }

        if (BLEManager::getInvalidCommand() == true)
        {
            BLEManager::sendInvalidCommandNotifications();
        }

        if (RTCManager::executionTimeCheck() == true)
        {
            xTaskCreate(runProfile, "runProfile", 2048, NULL, 1, &runProfileHandle);
        }

        RTCManager::printDateAndTime();
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

void profileHandlerTask(void *pvParameters)
{
    while (1)
    {
        if (MQTTManager::getProfileNameRecived() == true)
        {
            HTTPManager::getProfile();
            //  ProfileManager::runProfile();
            //  ProfileManager::runSunriseProfile();
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Check every 5 seconds
    }
}

void OTAUpdateTask(void *pvParameters)
{
    MQTTManager::setOtaUpdateAvailable(false);
    otaManage.beginHttpOTA();
    vTaskDelete(NULL);
}

void mqttHandlerTask(void *pvParameters)
{
    while (1)
    {
        MQTTManager::loop();
        // MQTTManager::publishLedStatePayload();
        vTaskDelay(pdMS_TO_TICKS(10)); // Check every 5 seconds
    }
}

void networkScanner(void *pvParameters)
{
    WiFiManager::networkScanner();
    vTaskDelete(NULL);
}

void runProfile(void *pvParameters)
{
    ProfileManager::runSunriseProfile();
    ProfileManager::runProfile();
    vTaskDelete(NULL);
}

void setup()
{
    Serial.begin(115200);

    FlashManager::init();
    BLEManager::init();
    RTCManager::init();
    MQTTManager::init();
    LED::init();

    xTaskCreate(mainTask, "mainTask", 2048, NULL, 2, NULL);
    xTaskCreate(mqttHandlerTask, "mqttHandlerTask", 2048, NULL, 1, NULL);
    xTaskCreate(profileHandlerTask, "profileHandlerTask", 2048, NULL, 1, NULL);
}

void loop() {}
