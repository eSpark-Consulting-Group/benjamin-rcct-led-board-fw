#pragma once


//PWM PIN DEFS
#define RED_PIN (PinName)PA_13  // Example: Casting to PinName
#define COOL_WHITE_PIN (PinName)PA_25
#define WARM_WHITE_PIN (PinName)PA_26 

//PWM CONFIG
#define FREQUENCY 1000.0f    // 1000 hz
#define TIME_PERIOD 1.0f/FREQUENCY
#define MAX_DUTY_VAL 65535.0f
#define MIN_DUTY_VAL 0.0f
#define SUNRISE_DURATION 60

//FLASH CONFIG
#define FLASH_SSID_OFFSET    0x0000  // SSID (String, max 64 bytes)
#define FLASH_PASS_OFFSET    0x0040  // Password (String, max 64 bytes)
#define MEMSET_BUFF_LEN_WIFICREDS 128
#define SSID_BUFF_LEN 64
#define PASS_BUFF_LEN 64

// #define FLASH_COLOR_OFFSET   0x0080  // Color (String, max 64 bytes)
// #define FLASH_TEMP_OFFSET    0x00C0  // Temperature (uint16_t, 2 bytes)
// #define FLASH_BRIGHT_OFFSET  0x00C2  // Brightness (uint16_t, 2 bytes)

//test JSON vals  
#define POWER         (bool)true
#define BRIGHTNESS     150
#define COLOR         "#FF0000"
#define EFFECT        "Blink"
#define SPEED            60  


//BRIGHTNESS
#define MIN_BRIGHTNESS   0.0f
#define MAX_BRIGHTNESS   255.0f
#define DEFAULT_BRIGHTNESS    0

//TEMPERATURE
#define MAX_TEMP    6500
#define MIN_TEMP    2700
#define DEFAULT_TEMP    0


//COLOR
#define MAX_COLOR     255.0f
#define MIN_COLOR     0.0f
#define DEFAULT_COLOR    "#FF0000"



//RTC
#define DEFAULT_YEAR 2025
#define DEFAULT_MONTH 1
#define DEFAULT_DAY 1

#define DEFAULT_HOUR 12
#define DEFAULT_MIN 0
#define DEFAULT_SEC 0

#define DESIRED_WAKEUP_HOUR 14
#define DESIRED_WAKEUP_MIN 13
#define DESIRED_WAKEUP_SEC 20  // set alarm as 10 seconds



//WIFI
#define WIFI_TIMEOUT_MS  5000  //ms



//BLE

#define WIFI_SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define WIFI_CREDENTIALS_CHAR_UUID "d752c5fb-1380-4cd5-b0ef-cac7d72cff20"
#define DEVICE_STATUS_CHAR_UUID    "e23a3e8c-1d8b-4b5a-88c9-fc3e2d5cbb23"
#define AVAILABLE_SSID_CHAR_UUID    "fa5942ef-6dcc-42e6-b6f7-0c5368f68583"

#define MAC_STRING_SIZE 18
#define WIFICREDSCHAR_BUFFER_LEN 180
#define DEVICESTATUSCHAR_BUFFER_LEN 180
#define AVAILABLESSIDCHAR_BUFFER_LEN 230


#define CMD_SCAN_START "scan_start"
#define STATUS_INVALID_COMMAND  "invalid command"
#define STATUS_SUCCESSFUL_SCAN  "scan_successful"
#define STATUS_FAILED_SCAN  "scan_failed"


//JSON

#define SSID_JSON_MAX_CHUNKS 50
#define SSID_JSON_MAX_SIZE 200 

//MQTT
#define SERVER_IP "184.169.181.179"
#define SERVER_PORT 1883
#define CLIENT_ID  "amebaClient"
#define CHUNK_TOPIC_1_SUB "controller/filetransfer/chunk/#"
#define CHUNK_TOPIC_2_SUB "controller/filetransfer/chunkCount"
#define CHUNK_TOPIC_3_SUB "controller/filetransfer/complete"
#define CONTROL_TOPIC_SUB   "led/control/"
#define PROFILE_TOPIC_SUB   "led/sunrise/"
#define OTA_TOPIC_SUB       "led/ota/"

#define DEVICE_TOPIC_PUB   "led/device/"


//HTTP

#define SERVER1_DOMAIN_NAME "api.ipgeolocation.io"
#define SERVER1_PATH "/timezone?apiKey="
#define SERVER1_API_KEY "1f70bad104504ec483f32a86e13aec18"
#define SERVER1_CONFIG_PATH SERVER1_PATH SERVER1_API_KEY
#define SERVER1_SUCCESS_CODE 200
#define SERVER1_TIME_PROPERTY "time_24"
#define SERVER1_DATE_PROPERTY "date"

#define SERVER2_DOMAIN_NAME "184.169.181.179"
#define SERVER2_PATH "/api/sunrise-profiles/Soft Glow"
#define SERVER2_PORT 3004



