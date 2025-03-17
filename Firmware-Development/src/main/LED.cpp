#include "LED.hpp"



static uint16_t brightnessRes;

LED& led = LED::getInstance();


void LED::init(){
    
    pwmout_init(&led.pwmCoolWhite, (PinName)COOL_WHITE_PIN);
    pwmout_period(&led.pwmCoolWhite, TIME_PERIOD);   

    pwmout_init(&led.pwmRed, (PinName)RED_PIN);
    pwmout_period(&led.pwmRed, TIME_PERIOD); 


    pwmout_init(&led.pwmWarmWhite, (PinName)WARM_WHITE_PIN);
    pwmout_period(&led.pwmWarmWhite, TIME_PERIOD);

    pwmout_write(&led.pwmCoolWhite, 0);
    pwmout_write(&led.pwmRed, 0);
    pwmout_write(&led.pwmWarmWhite, 0);
}

void LED::hexToCRW(const String& hex,  int& coolWhite, int& red, int& warmWhite) {
    if (hex.length() != 7 || hex[0] != '#') return;  // Ensure valid hex format

    coolWhite = strtol(hex.substring(1, 3).c_str(), nullptr, 16);
    red = strtol(hex.substring(3, 5).c_str(), nullptr, 16);
    warmWhite = strtol(hex.substring(5, 7).c_str(), nullptr, 16);
    Serial.print("RGB: "); Serial.print(coolWhite); Serial.print(", "); Serial.print(red); Serial.print(", "); Serial.println(warmWhite);

}

void LED::configBrightness(){
    Serial.print("power :") ; Serial.println(led.config.power);

        if (led.config.brightness > MAX_BRIGHTNESS) {
            led.config.brightness=MAX_BRIGHTNESS;
        }
        else if (led.config.brightness < MIN_BRIGHTNESS){
            led.config.brightness = MIN_BRIGHTNESS;
        }
        brightnessRes =map(led.config.brightness, MIN_BRIGHTNESS ,MAX_BRIGHTNESS ,MIN_PWM_VAL ,MAX_PWM_VAL) ;
        Serial.println(brightnessRes);

        pwmout_write(&led.pwmCoolWhite, brightnessRes/MAX_PWM_VAL);
        pwmout_write(&led.pwmRed, brightnessRes/MAX_PWM_VAL);
        pwmout_write(&led.pwmWarmWhite, brightnessRes/MAX_PWM_VAL);

}

void LED::configColor(){
        static uint16_t coolWhitePWM;
        static uint16_t redPWM; 
        static uint16_t warmWhitePWM;

        Serial.print("[config color] red"); Serial.println(led.config.color.red);

        coolWhitePWM= map(led.config.color.coolWhite , MIN_COLOR, MAX_COLOR,  MIN_PWM_VAL ,brightnessRes);
        redPWM = map(led.config.color.red, MIN_COLOR, MAX_COLOR,  MIN_PWM_VAL ,brightnessRes);
        warmWhitePWM  = map(led.config.color.warmWhite, MIN_COLOR ,MAX_COLOR,  MIN_PWM_VAL ,brightnessRes);


        Serial.print("redPWM:"); Serial.println(redPWM);
        Serial.print("brightness res:"); Serial.println(brightnessRes);
        // Apply PWM output
        pwmout_write(&led.pwmCoolWhite, coolWhitePWM  / MAX_PWM_VAL);
        pwmout_write(&led.pwmRed,  redPWM / MAX_PWM_VAL);
        pwmout_write(&led.pwmWarmWhite, warmWhitePWM / MAX_PWM_VAL);
}


void LED::configureLED(){

    MQTTManager::ledConfigRecievedFlag(true,false,false);

    LED::hexToCRW(led.config.color.colorString, 
                     led.config.color.coolWhite,
                        led.config.color.red, 
                            led.config.color.warmWhite); 
    Serial.print("[configureLED ]red"); Serial.println(led.config.color.red);

    if(led.config.power == true ){    
            LED::configBrightness();
            LED::configColor();
        }
    else if(led.config.power ==  false){
        pwmout_write(&led.pwmCoolWhite, 0);
        pwmout_write(&led.pwmRed, 0);
        pwmout_write(&led.pwmWarmWhite, 0);
    }


}



