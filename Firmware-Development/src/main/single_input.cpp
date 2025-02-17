#include "single_input.hpp"


// PWM STRCUTURES FOR ANIMATIONS
// pwmout_t pwm_red;
// pwmout_t  pwm_cool_white;
// pwmout_t  pwm_warm_white;

bool single_input_recieved=true;


pwmout_t single_input_pwm::pwm_red;
pwmout_t single_input_pwm::pwm_cool_white;
pwmout_t single_input_pwm::pwm_warm_white;

const float time_period = 1/freq; // time period
uint16_t pwm_value1 = 52276; // 50% duty cycle (32768 / 65535 = 0.5)
uint16_t pwm_value2 = 3000;
uint16_t pwm_value3 = 200;




void single_input_pwm::PWM_init(){
    pwmout_init(&pwm_red, (PinName)RED_PIN);
    pwmout_period(&pwm_red, time_period); 

    pwmout_init(&pwm_cool_white, (PinName)COOL_WHITE_PIN);
    pwmout_period(&pwm_cool_white, time_period);   

    pwmout_init(&pwm_warm_white, (PinName)WARM_WHITE_PIN);
    pwmout_period(&pwm_warm_white, time_period);
}


// void single_input_pwm::run_sunrise() {
//   Serial.println("Starting sunrise sequence...");
//   int steps = sizeof(warm_white_values) / sizeof(warm_white_values[0]);
//   for (int i = 0; i < steps; i++) {
//     // if (stopRequested) {
//     //   Serial.println("Sunrise sequence interrupted.");
//     //   return;
//     // }

//     float warm_value = warm_white_values[i];
//     float cool_value = cool_white_values[i];
//     float red_value = red_values[i];

//     float duty_cycle1_red = (float)red_value / MAX_DUTY_VAL;
//     float duty_cycle2_cool = (float)cool_value / MAX_DUTY_VAL;
//     float duty_cycle3_warm = (float)warm_value / MAX_DUTY_VAL;


//     // Update the PWM values
//     set_led_values(duty_cycle3_warm, duty_cycle2_cool, duty_cycle1_red);
//     vTaskDelay(pdMS_TO_TICKS((sunrise_duration*1000 )/steps));
//     // delay((sunrise_duration * 1000) / steps);

//     // // Print current values to the terminal
//     // Serial.print("Step ");
//     // Serial.print(i + 1);
//     // Serial.println(":");
//     // Serial.print("Warm White: ");
//     // Serial.println(warm_value);
//     // Serial.print("Cool White: ");
//     // Serial.println(cool_value);
//     // Serial.print("Red: ");
//     // Serial.println(red_value);
//     // Serial.println();
//   }
//   Serial.println("Sunrise sequence completed.");
// }

        uint16_t brightness_res;
        uint16_t cool_temp_res;
        uint16_t warm_temp_res;
        uint16_t color_res; 

bool single_input_pwm::single_input_recieved_flag( bool write , bool write_val ,bool read){
    if(write == true){
        single_input_recieved= write_val;
    }
    else if(read == true){
      return(single_input_recieved);
    }
}

void single_input_pwm::hexToRGB(const String& hex, int& red, int& green, int& blue) {
    if (hex.length() != 7 || hex[0] != '#') return;  // Ensure valid hex format

    red = strtol(hex.substring(1, 3).c_str(), nullptr, 16);
    green = strtol(hex.substring(3, 5).c_str(), nullptr, 16);
    blue = strtol(hex.substring(5, 7).c_str(), nullptr, 16);
    Serial.print("RGB: "); Serial.print(red); Serial.print(", "); Serial.print(green); Serial.print(", "); Serial.println(blue);

}

void single_input_pwm::config_brightness(){
        brightness_res =map(single_input_pwm::getInstance().config.brightness, MIN_BRIGHTNESS ,MAX_BRIGHTNESS ,MIN_DUTY_VAL ,MAX_DUTY_VAL) ;
        pwmout_write(&pwm_warm_white, brightness_res/MAX_DUTY_VAL);
        pwmout_write(&pwm_cool_white, brightness_res/MAX_DUTY_VAL);
        pwmout_write(&pwm_red, brightness_res/MAX_DUTY_VAL);

}

void single_input_pwm::config_color(){
        color_res= map(single_input_pwm::getInstance().RGB.red, MAX_COLOR, MIN_COLOR ,MIN_DUTY_VAL ,brightness_res);
        pwmout_write(&pwm_red, color_res/MAX_DUTY_VAL);
}

void single_input_pwm::config_temperature(){

    // Adjust Cool/Warm white based on temperature (2700K to 6500K)
      cool_temp_res = map(single_input_pwm::getInstance().config.temperature, MIN_TEMP, MAX_TEMP, MIN_DUTY_VAL, brightness_res);
      warm_temp_res = map(single_input_pwm::getInstance().config.temperature, MIN_TEMP, MAX_TEMP, brightness_res, MIN_DUTY_VAL);
            
      pwmout_write(&pwm_warm_white, warm_temp_res/MAX_DUTY_VAL);
      pwmout_write(&pwm_cool_white, cool_temp_res/MAX_DUTY_VAL);

}

void single_input_pwm::config_bright_color_temp(){
     single_input_recieved=false;
     flashman::load_single_input_vals(single_input_pwm::getInstance().config.color , 
                                          single_input_pwm::getInstance().config.temperature, 
                                             single_input_pwm::getInstance().config.brightness);

    single_input_pwm::hexToRGB(single_input_pwm::getInstance().config.color, 
                                  single_input_pwm::getInstance().RGB.red, 
                                      single_input_pwm::getInstance().RGB.green,
                                         single_input_pwm::getInstance().RGB.blue); 

    single_input_pwm::config_brightness();
    single_input_pwm::config_color();
    single_input_pwm::config_temperature();
}


// void single_input_pwm::set_led_values(float warm_value, float cool_value, float red_value) {

//   pwmout_write(&pwm_warm_white, warm_value);
//   pwmout_write(&pwm_red, red_value);
//   pwmout_write(&pwm_cool_white, cool_value);
//   // Serial.print("Setting Warm White: ");
//   // Serial.print(warm_value);
//   // Serial.print(", Cool White: ");
//   // Serial.print(cool_value);
//   // Serial.print(", Red: ");
//   // Serial.println(red_value);
// }
