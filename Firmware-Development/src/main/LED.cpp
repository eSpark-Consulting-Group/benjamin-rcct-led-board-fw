#include "LED.hpp"

static uint16_t brightnessRes;

void LED::init()
{

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

void LED::hexToCRW(const String &hex, int &coolWhite, int &red, int &warmWhite)
{
    if (hex.length() != 7 || hex[0] != '#')
        return; // Ensure valid hex format

    coolWhite = strtol(hex.substring(1, 3).c_str(), nullptr, 16);
    red = strtol(hex.substring(3, 5).c_str(), nullptr, 16);
    warmWhite = strtol(hex.substring(5, 7).c_str(), nullptr, 16);
    Serial.print("RGB: ");
    Serial.print(coolWhite);
    Serial.print(", ");
    Serial.print(red);
    Serial.print(", ");
    Serial.println(warmWhite);
}

void LED::configBrightness()
{
    LED &led = LED::getInstance();
    Serial.print("power :");
    Serial.println(led.ledconfig.power);

    if (led.ledconfig.brightness > MAX_BRIGHTNESS)
    {
        led.ledconfig.brightness = MAX_BRIGHTNESS;
    }
    else if (led.ledconfig.brightness < MIN_BRIGHTNESS)
    {
        led.ledconfig.brightness = MIN_BRIGHTNESS;
    }
    brightnessRes = map(led.ledconfig.brightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS, MIN_PWM_VAL, MAX_PWM_VAL);
    Serial.println(brightnessRes);

    pwmout_write(&led.pwmCoolWhite, brightnessRes / MAX_PWM_VAL);
    pwmout_write(&led.pwmRed, brightnessRes / MAX_PWM_VAL);
    pwmout_write(&led.pwmWarmWhite, brightnessRes / MAX_PWM_VAL);
}

void LED::configColor()
{
    static uint16_t coolWhitePWM;
    static uint16_t redPWM;
    static uint16_t warmWhitePWM;

    Serial.print("[config color] red");
    Serial.println(LED::getInstance().ledconfig.color.red);

    coolWhitePWM = map(LED::getInstance().ledconfig.color.coolWhite, MIN_COLOR, MAX_COLOR, MIN_PWM_VAL, brightnessRes);
    redPWM = map(LED::getInstance().ledconfig.color.red, MIN_COLOR, MAX_COLOR, MIN_PWM_VAL, brightnessRes);
    warmWhitePWM = map(LED::getInstance().ledconfig.color.warmWhite, MIN_COLOR, MAX_COLOR, MIN_PWM_VAL, brightnessRes);

    Serial.print("redPWM:");
    Serial.println(redPWM);
    Serial.print("brightness res:");
    Serial.println(brightnessRes);
    // Apply PWM output
    pwmout_write(&LED::getInstance().pwmCoolWhite, coolWhitePWM / MAX_PWM_VAL);
    pwmout_write(&LED::getInstance().pwmRed, redPWM / MAX_PWM_VAL);
    pwmout_write(&LED::getInstance().pwmWarmWhite, warmWhitePWM / MAX_PWM_VAL);
}

void LED::configureLED()
{

    MQTTManager::setLedConfigReceived(false);

    LED::hexToCRW(LED::getInstance().ledconfig.color.colorString,
                  LED::getInstance().ledconfig.color.coolWhite,
                  LED::getInstance().ledconfig.color.red,
                  LED::getInstance().ledconfig.color.warmWhite);
    Serial.print("[configureLED ]red");
    Serial.println(LED::getInstance().ledconfig.color.red);

    if (LED::getInstance().ledconfig.power == true)
    {
        LED::configBrightness();
        LED::configColor();
    }
    else if (LED::getInstance().ledconfig.power == false)
    {
        pwmout_write(&LED::getInstance().pwmCoolWhite, 0);
        pwmout_write(&LED::getInstance().pwmRed, 0);
        pwmout_write(&LED::getInstance().pwmWarmWhite, 0);
    }
}
