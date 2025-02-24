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




class RTCManager{
  public:

    static RTCManager& getInstance() {
        static RTCManager instance;
        return instance;
    }

    alarm_t alarm;
    struct tm *timeinfo;
    CurrentTimeInfo crntimeinfo;

    static void init();
    static void disableAlarm(void);
    static void enableAlarm();
    static void updateRTCTime();

  private:
    RTCManager() {}  // Private constructor (singleton)
    static void rtcHandler(void) ;
    static void printStringTime(void);

};











