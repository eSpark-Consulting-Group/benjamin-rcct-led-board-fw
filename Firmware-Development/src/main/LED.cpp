#include "LED.hpp"


bool single_input_recieved=false;

uint16_t brightness_res;
uint16_t cool_temp_res;
uint16_t warm_temp_res;
uint16_t color_res; 


// pwmout_t LED::pwm_red;
// pwmout_t LED::pwm_cool_white;
// pwmout_t LED::pwm_warm_white;


void LED::PWM_init(){
    pwmout_init(&LED::getInstance().pwm_red, (PinName)RED_PIN);
    pwmout_period(&LED::getInstance().pwm_red, TIME_PERIOD); 

    pwmout_init(&LED::getInstance().pwm_cool_white, (PinName)COOL_WHITE_PIN);
    pwmout_period(&LED::getInstance().pwm_cool_white, TIME_PERIOD);   

    pwmout_init(&LED::getInstance().pwm_warm_white, (PinName)WARM_WHITE_PIN);
    pwmout_period(&LED::getInstance().pwm_warm_white, TIME_PERIOD);
}


bool LED::single_input_recieved_flag( bool write , bool write_val ,bool read){
    if(write == true){
        single_input_recieved= write_val;
    }
    else if(read == true){
      return(single_input_recieved);
    }
    // return ;
}

void LED::hexToRGB(const String& hex, int& red, int& green, int& blue) {
    if (hex.length() != 7 || hex[0] != '#') return;  // Ensure valid hex format

    red = strtol(hex.substring(1, 3).c_str(), nullptr, 16);
    green = strtol(hex.substring(3, 5).c_str(), nullptr, 16);
    blue = strtol(hex.substring(5, 7).c_str(), nullptr, 16);
    Serial.print("RGB: "); Serial.print(red); Serial.print(", "); Serial.print(green); Serial.print(", "); Serial.println(blue);

}

void LED::config_brightness(){
        if (LED::getInstance().config.brightness > MAX_BRIGHTNESS) {
            LED::getInstance().config.brightness=MAX_BRIGHTNESS;
        }
        else if (LED::getInstance().config.brightness < MIN_BRIGHTNESS){
            LED::getInstance().config.brightness = MIN_BRIGHTNESS;
        }
        brightness_res =map(LED::getInstance().config.brightness, MIN_BRIGHTNESS ,MAX_BRIGHTNESS ,MIN_DUTY_VAL ,MAX_DUTY_VAL) ;
        Serial.println(brightness_res);

        pwmout_write(&LED::getInstance().pwm_warm_white, brightness_res/MAX_DUTY_VAL);
        pwmout_write(&LED::getInstance().pwm_cool_white, brightness_res/MAX_DUTY_VAL);
        pwmout_write(&LED::getInstance().pwm_red, brightness_res/MAX_DUTY_VAL);

}

void LED::config_color(){
        // color_res= map(LED::getInstance().RGB.red, MAX_COLOR, MIN_COLOR ,MIN_DUTY_VAL ,brightness_res);
        // pwmout_write(&LED::getInstance().pwm_red, color_res/MAX_DUTY_VAL);

        // float R_norm = LED::getInstance().ledconfig.color.red/ 255.0f;
        // float G_norm = LED::getInstance().ledconfig.color.green / 255.0f;
        // float B_norm = LED::getInstance().ledconfig.color.blue / 255.0f;

        // // Compute R, W, C based on RGB balance 
        // int R_out = (R_norm * (brightness_res/MAX_DUTY_VAL)) *255;
        // int W_out = ((0.5f * R_norm + 0.8f * G_norm) * (brightness_res/MAX_DUTY_VAL) )*255;
        // int C_out = ((0.2f * G_norm + 0.8f * B_norm) * (brightness_res/MAX_DUTY_VAL)) *255;

        // // Ensure values are within PWM range
        // uint16_t R_PWM = (uint16_t)(R_out * 65535);
        // uint16_t W_PWM= (uint16_t)(W_out * 65535);
        // uint16_t C_PWM = (uint16_t)(C_out * 65535);

        // // Apply PWM output
        // pwmout_write(&LED::getInstance().ledconfig.color.red, R_PWM / MAX_DUTY_VAL);
        // pwmout_write(&LED::getInstance().ledconfig.color.green, W_PWM / MAX_DUTY_VAL);
        // pwmout_write(&LED::getInstance().ledconfig.color.blue, C_PWM / MAX_DUTY_VAL);
}

// void LED::config_temperature(){
//       if (LED::getInstance().config.temperature > MAX_TEMP) {
//         LED::getInstance().config.temperature=MAX_TEMP;
//         }
//         else if (LED::getInstance().config.temperature < MIN_TEMP){
//             LED::getInstance().config.temperature = MIN_TEMP;
//         }
//     // Adjust Cool/Warm white based on temperature (2700K to 6500K)
//       cool_temp_res = map(LED::getInstance().config.temperature, MIN_TEMP, MAX_TEMP, MIN_DUTY_VAL, brightness_res);
//       warm_temp_res = map(LED::getInstance().config.temperature, MIN_TEMP, MAX_TEMP, brightness_res, MIN_DUTY_VAL);
            
//       pwmout_write(&LED::getInstance().pwm_warm_white, warm_temp_res/MAX_DUTY_VAL);
//       pwmout_write(&LED::getInstance().pwm_cool_white, cool_temp_res/MAX_DUTY_VAL);

// }

void LED::config_bright_color_temp(){
     single_input_recieved=false;
    //  FlashManager::load_single_input_vals(LED::getInstance().config.color , 
    //                                       LED::getInstance().config.temperature, 
    //                                          LED::getInstance().config.brightness);

    LED::hexToRGB(LED::getInstance().ledconfig.color.colorString, 
                    LED::getInstance().ledconfig.color.red, 
                        LED::getInstance().ledconfig.color.green,
                            LED::getInstance().ledconfig.color.blue); 

    LED::config_brightness();
    LED::config_color();

}



