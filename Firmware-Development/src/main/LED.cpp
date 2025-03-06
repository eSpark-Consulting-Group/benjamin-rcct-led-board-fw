#include "LED.hpp"



static uint16_t brightnessRes;




void LED::init(){
    
    pwmout_init(&LED::getInstance().pwmCoolWhite, (PinName)COOL_WHITE_PIN);
    pwmout_period(&LED::getInstance().pwmCoolWhite, TIME_PERIOD);   

    pwmout_init(&LED::getInstance().pwmRed, (PinName)RED_PIN);
    pwmout_period(&LED::getInstance().pwmRed, TIME_PERIOD); 


    pwmout_init(&LED::getInstance().pwmWarmWhite, (PinName)WARM_WHITE_PIN);
    pwmout_period(&LED::getInstance().pwmWarmWhite, TIME_PERIOD);

    pwmout_write(&LED::getInstance().pwmCoolWhite, 0);
    pwmout_write(&LED::getInstance().pwmRed, 0);
    pwmout_write(&LED::getInstance().pwmWarmWhite, 0);
}




void LED::hexToCRW(const String& hex,  int& coolWhite, int& red, int& warmWhite) {
    if (hex.length() != 7 || hex[0] != '#') return;  // Ensure valid hex format

    coolWhite = strtol(hex.substring(3, 5).c_str(), nullptr, 16);
    red = strtol(hex.substring(1, 3).c_str(), nullptr, 16);
    warmWhite = strtol(hex.substring(5, 7).c_str(), nullptr, 16);
    Serial.print("RGB: "); Serial.print(coolWhite); Serial.print(", "); Serial.print(red); Serial.print(", "); Serial.println(warmWhite);

}

void LED::configBrightness(){
    Serial.print("power :") ; Serial.println(LED::getInstance().ledconfig.power);

        if (LED::getInstance().ledconfig.brightness > MAX_BRIGHTNESS) {
            LED::getInstance().ledconfig.brightness=MAX_BRIGHTNESS;
        }
        else if (LED::getInstance().ledconfig.brightness < MIN_BRIGHTNESS){
            LED::getInstance().ledconfig.brightness = MIN_BRIGHTNESS;
        }
        brightnessRes =map(LED::getInstance().ledconfig.brightness, MIN_BRIGHTNESS ,MAX_BRIGHTNESS ,MIN_DUTY_VAL ,MAX_DUTY_VAL) ;
        Serial.println(brightnessRes);

        pwmout_write(&LED::getInstance().pwmCoolWhite, brightnessRes/MAX_DUTY_VAL);
        pwmout_write(&LED::getInstance().pwmRed, brightnessRes/MAX_DUTY_VAL);
        pwmout_write(&LED::getInstance().pwmWarmWhite, brightnessRes/MAX_DUTY_VAL);

}

void LED::configColor(){
        static uint16_t coolWhitePWM;
        static uint16_t redPWM; 
        static uint16_t warmWhitePWM;

        Serial.print("[config color] red"); Serial.println(LED::getInstance().ledconfig.color.red);

        coolWhitePWM= map(LED::getInstance().ledconfig.color.coolWhite , MIN_COLOR, MAX_COLOR,  MIN_DUTY_VAL ,brightnessRes);
        redPWM = map(LED::getInstance().ledconfig.color.red, MIN_COLOR, MAX_COLOR,  MIN_DUTY_VAL ,brightnessRes);
        warmWhitePWM  = map(LED::getInstance().ledconfig.color.warmWhite, MIN_COLOR ,MAX_COLOR,  MIN_DUTY_VAL ,brightnessRes);


        Serial.print("redPWM:"); Serial.println(redPWM);
        Serial.print("brightness res:"); Serial.println(brightnessRes);
        // Apply PWM output
        pwmout_write(&LED::getInstance().pwmCoolWhite, coolWhitePWM  / MAX_DUTY_VAL);
        pwmout_write(&LED::getInstance().pwmRed,  redPWM / MAX_DUTY_VAL);
        pwmout_write(&LED::getInstance().pwmWarmWhite, warmWhitePWM / MAX_DUTY_VAL);
}



void LED::configureLED(){

    MQTTManager::ledConfigRecievedFlag(true,false,false);

    LED::hexToCRW(LED::getInstance().ledconfig.color.colorString, 
                     LED::getInstance().ledconfig.color.coolWhite,
                        LED::getInstance().ledconfig.color.red, 
                            LED::getInstance().ledconfig.color.warmWhite); 
    Serial.print("[configureLED ]red"); Serial.println(LED::getInstance().ledconfig.color.red);

    if(LED::getInstance().ledconfig.power == true ){    
            LED::configBrightness();
            LED::configColor();
        }
    else if(LED::getInstance().ledconfig.power ==  false){
        pwmout_write(&LED::getInstance().pwmCoolWhite, 0);
        pwmout_write(&LED::getInstance().pwmRed, 0);
        pwmout_write(&LED::getInstance().pwmWarmWhite, 0);
    }


}



