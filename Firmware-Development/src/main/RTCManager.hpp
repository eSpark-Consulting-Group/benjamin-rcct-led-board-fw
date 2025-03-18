#pragma once

#include <stdio.h>
#include <time.h>
#include "rtc.h"

#include "config.hpp"


typedef struct{
  uint16_t year = DEFAULT_YEAR;
  uint8_t month = DEFAULT_MONTH;
  uint8_t day = DEFAULT_DAY;
  uint8_t hour = DEFAULT_HOUR;
  uint8_t min = DEFAULT_MIN;
  uint8_t sec = DEFAULT_SEC;

}CurrentTimeInfo;

typedef struct{

  uint8_t desiredHour = DESIRED_WAKEUP_HOUR;
  uint8_t desiredMin = DESIRED_WAKEUP_MIN;
  uint8_t desiredSec = DESIRED_WAKEUP_SEC;

}DesiredWakeUpTime;

typedef struct{

  uint8_t executionHour ;
  uint8_t executionMin ;
  uint8_t executionSec ;

}ExecutionTime;



class RTCManager{
  public:

    static RTCManager& getInstance() {
        static RTCManager instance;
        return instance;
    }

    alarm_t alarm;
    static struct tm *timeinfo;

    CurrentTimeInfo crntimeinfo;
    DesiredWakeUpTime destimeinfo;
    ExecutionTime exectime;

    static void init();
    static void disableAlarm(void);
    static void enableAlarm();
    static void syncRTCDateAndTime();
    static void printDateAndTime(void);
    static bool executionTimeCheck();
    static void setExecutionTime();

  private:
    RTCManager() {}  // Private constructor (singleton)
    static void rtcHandler(void) ;

};











