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
#define DEFAULT_BRIGHTNESS    1

//TEMPERATURE
#define MAX_TEMP    6500
#define MIN_TEMP    2700
#define DEFAULT_TEMP    4600


//COLOR
#define MAX_COLOR     255.0f
#define MIN_COLOR     0.0f
#define DEFAULT_COLOR    "#FF0000"







