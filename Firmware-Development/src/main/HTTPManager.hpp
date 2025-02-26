#pragma once


// #include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HttpClient.h>
#include <set>

#include "JSONManager.hpp"
#include "RTCManager.hpp"
#include "config.hpp"



class HTTPManager{
    public:
        

        static void httpGetAPIResponse() ;
        static void extractDateAndTimeFromAPIResponse();

    private:
        static void extractTimeFromAPIResponse();
        static void extractDateFromAPIResponse();


};