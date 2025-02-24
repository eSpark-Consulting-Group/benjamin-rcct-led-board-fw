#include "RTCManager.hpp"


RTC rtc;
int32_t seconds;



void RTCManager::init(){
    rtc.Init();
    int initTime = rtc.SetEpoch( RTCManager::getInstance().crntimeinfo.year, 
                                    RTCManager::getInstance().crntimeinfo.month, 
                                        RTCManager::getInstance().crntimeinfo.day, 
                                            RTCManager::getInstance().crntimeinfo.hour, 
                                                RTCManager::getInstance().crntimeinfo.min, 
                                                    RTCManager::getInstance().crntimeinfo.sec);  // convert user time to seconds from 1970.1.1 00:00:00
    
    rtc.Write(initTime);

}

void RTCManager::enableAlarm() {
    RTCManager::getInstance().alarm.yday = RTCManager::getInstance().crntimeinfo.day ;  //  default alarmDay to be zero
    RTCManager::getInstance().alarm.hour = ALARM_HOUR;
    RTCManager::getInstance().alarm.min = ALARM_MIN;
    RTCManager::getInstance().alarm.sec = ALARM_SEC;
    rtc_set_alarm(&RTCManager::getInstance().alarm, (alarm_irq_handler)&rtcHandler);

}

void RTCManager::disableAlarm(void) {
    rtc_disable_alarm();
    Serial.println("Alarm disabled");
}

void RTCManager::rtcHandler(void) {
    Serial.println("====   Alarm Matched   ====");
    while (1);
}


void RTCManager::updateRTCTime(){
    seconds = rtc.Read();
    RTCManager::getInstance().timeinfo = localtime(&seconds);
    printStringTime();

    rtc.Wait(1);

}


void RTCManager::printStringTime(void) {
    RTCManager::getInstance().timeinfo = localtime(&seconds);
    Serial.print("Time:       ");
    Serial.print(RTCManager::getInstance().timeinfo->tm_year + 1900);
    Serial.print("-");
    Serial.print(RTCManager::getInstance().timeinfo->tm_mon + 1);
    Serial.print("-");
    Serial.print(RTCManager::getInstance().timeinfo->tm_mday);
    Serial.print(" ");
    Serial.print(RTCManager::getInstance().timeinfo->tm_hour);
    Serial.print(":");
    Serial.print(RTCManager::getInstance().timeinfo->tm_min);
    Serial.print(":");
    Serial.println(RTCManager::getInstance().timeinfo->tm_sec);
}


