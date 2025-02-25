#include "SingleInput.hpp"


bool single_input_recieved=true;

uint16_t brightness_res;
uint16_t cool_temp_res;
uint16_t warm_temp_res;
uint16_t color_res; 


// pwmout_t SingleInput::pwm_red;
// pwmout_t SingleInput::pwm_cool_white;
// pwmout_t SingleInput::pwm_warm_white;

// const float time_period = 1/FREQUENCY; // time period


void SingleInput::PWM_init(){
    pwmout_init(&SingleInput::getInstance().pwm_red, (PinName)RED_PIN);
    pwmout_period(&SingleInput::getInstance().pwm_red, TIME_PERIOD); 

    pwmout_init(&SingleInput::getInstance().pwm_cool_white, (PinName)COOL_WHITE_PIN);
    pwmout_period(&SingleInput::getInstance().pwm_cool_white, TIME_PERIOD);   

    pwmout_init(&SingleInput::getInstance().pwm_warm_white, (PinName)WARM_WHITE_PIN);
    pwmout_period(&SingleInput::getInstance().pwm_warm_white, TIME_PERIOD);
}


bool SingleInput::single_input_recieved_flag( bool write , bool write_val ,bool read){
    if(write == true){
        single_input_recieved= write_val;
    }
    else if(read == true){
      return(single_input_recieved);
    }
    return;
}

void SingleInput::hexToRGB(const String& hex, int& red, int& green, int& blue) {
    if (hex.length() != 7 || hex[0] != '#') return;  // Ensure valid hex format

    red = strtol(hex.substring(1, 3).c_str(), nullptr, 16);
    green = strtol(hex.substring(3, 5).c_str(), nullptr, 16);
    blue = strtol(hex.substring(5, 7).c_str(), nullptr, 16);
    Serial.print("RGB: "); Serial.print(red); Serial.print(", "); Serial.print(green); Serial.print(", "); Serial.println(blue);

}

void SingleInput::config_brightness(){
        if (SingleInput::getInstance().config.brightness > MAX_BRIGHTNESS) {
            SingleInput::getInstance().config.brightness=MAX_BRIGHTNESS;
        }
        else if (SingleInput::getInstance().config.brightness < MIN_BRIGHTNESS){
            SingleInput::getInstance().config.brightness = MIN_BRIGHTNESS;
        }
        brightness_res =map(SingleInput::getInstance().config.brightness, MIN_BRIGHTNESS ,MAX_BRIGHTNESS ,MIN_DUTY_VAL ,MAX_DUTY_VAL) ;
        Serial.println(brightness_res);

        pwmout_write(&SingleInput::getInstance().pwm_warm_white, brightness_res/MAX_DUTY_VAL);
        pwmout_write(&SingleInput::getInstance().pwm_cool_white, brightness_res/MAX_DUTY_VAL);
        pwmout_write(&SingleInput::getInstance().pwm_red, brightness_res/MAX_DUTY_VAL);

}

void SingleInput::config_color(){
        color_res= map(SingleInput::getInstance().RGB.red, MAX_COLOR, MIN_COLOR ,MIN_DUTY_VAL ,brightness_res);
        pwmout_write(&SingleInput::getInstance().pwm_red, color_res/MAX_DUTY_VAL);
}

void SingleInput::config_temperature(){
      if (SingleInput::getInstance().config.temperature > MAX_TEMP) {
            SingleInput::getInstance().config.temperature=MAX_TEMP;
        }
        else if (SingleInput::getInstance().config.temperature < MIN_TEMP){
            SingleInput::getInstance().config.temperature = MIN_TEMP;
        }
    // Adjust Cool/Warm white based on temperature (2700K to 6500K)
      cool_temp_res = map(SingleInput::getInstance().config.temperature, MIN_TEMP, MAX_TEMP, MIN_DUTY_VAL, brightness_res);
      warm_temp_res = map(SingleInput::getInstance().config.temperature, MIN_TEMP, MAX_TEMP, brightness_res, MIN_DUTY_VAL);
            
      pwmout_write(&SingleInput::getInstance().pwm_warm_white, warm_temp_res/MAX_DUTY_VAL);
      pwmout_write(&SingleInput::getInstance().pwm_cool_white, cool_temp_res/MAX_DUTY_VAL);

}

void SingleInput::config_bright_color_temp(){
     single_input_recieved=false;
    //  FlashManager::load_single_input_vals(SingleInput::getInstance().config.color , 
    //                                       SingleInput::getInstance().config.temperature, 
    //                                          SingleInput::getInstance().config.brightness);

    SingleInput::hexToRGB(SingleInput::getInstance().config.color, 
                                  SingleInput::getInstance().RGB.red, 
                                      SingleInput::getInstance().RGB.green,
                                         SingleInput::getInstance().RGB.blue); 

    SingleInput::config_brightness();
    SingleInput::config_color();
    SingleInput::config_temperature();
}



