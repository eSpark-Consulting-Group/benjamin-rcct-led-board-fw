#include "RTCManager.hpp"


RTC rtc;


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


void RTCManager::setExecutionTime(){
    uint8_t hr = RTCManager::getInstance().destimeinfo.desiredHour;
    uint8_t min = RTCManager::getInstance().destimeinfo.desiredMin;
    uint8_t sec = RTCManager::getInstance().destimeinfo.desiredSec;
    int32_t executeTimeInSec = (hr*3600 + min*60 +sec) - SUNRISE_DURATION;

        // Handle underflow (if subtractedSeconds becomes negative)
    if (executeTimeInSec < 0) {
        executeTimeInSec += 24 * 3600;  // Add 24 hours in seconds
    }
    RTCManager::getInstance().exectime.executionHour= executeTimeInSec / 3600;
    RTCManager::getInstance().exectime.executionMin = (executeTimeInSec % 3600) / 60;
    RTCManager::getInstance().exectime.executionSec = executeTimeInSec % 60;
    Serial.print("Execute hour:");
    Serial.println(RTCManager::getInstance().exectime.executionHour);
    Serial.print("Execute min:");
    Serial.println(RTCManager::getInstance().exectime.executionMin);
    Serial.print("Execute sec:");
    Serial.println(RTCManager::getInstance().exectime.executionSec);
}

void RTCManager::syncRTCDateAndTime(){
    int initTime = rtc.SetEpoch( RTCManager::getInstance().crntimeinfo.year, 
                                    RTCManager::getInstance().crntimeinfo.month, 
                                        RTCManager::getInstance().crntimeinfo.day, 
                                            RTCManager::getInstance().crntimeinfo.hour, 
                                                RTCManager::getInstance().crntimeinfo.min, 
                                                    RTCManager::getInstance().crntimeinfo.sec);  // convert user time to seconds from 1970.1.1 00:00:00
    
    rtc.Write(initTime);
}


void RTCManager::printDateAndTime(void) {
    int32_t seconds;
    seconds = rtc.Read();
    RTCManager::getInstance().timeinfo = localtime(&seconds);

    if (RTCManager::getInstance().timeinfo != nullptr) {
      Serial.print("Time:       ");
      Serial.print(RTCManager::getInstance().timeinfo->tm_year + 1900);
      Serial.print("-");
      Serial.print(RTCManager::getInstance().timeinfo->tm_mon +1 );
      Serial.print("-");
      Serial.print(RTCManager::getInstance().timeinfo->tm_mday);
      Serial.print(" ");
      Serial.print(RTCManager::getInstance().timeinfo->tm_hour);
      Serial.print(":");
      Serial.print(RTCManager::getInstance().timeinfo->tm_min);
      Serial.print(":");
      Serial.println(RTCManager::getInstance().timeinfo->tm_sec);
      } else {
        Serial.println("Failed to convert time.");
    }    
    // rtc.Wait(1);
}

bool RTCManager::executionTimeCheck(){
    int32_t seconds;
    seconds = rtc.Read();
    RTCManager::getInstance().timeinfo = localtime(&seconds);
    if((RTCManager::getInstance().timeinfo->tm_hour)  == (RTCManager::getInstance().exectime.executionHour) &&
            (RTCManager::getInstance().timeinfo->tm_min) == (RTCManager::getInstance().exectime.executionMin) && 
              (RTCManager::getInstance().timeinfo->tm_sec) == (RTCManager::getInstance().exectime.executionSec) ){

                return true;
              }
              
            else{
                return false;
            }
        
}
