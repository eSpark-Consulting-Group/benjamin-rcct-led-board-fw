#pragma once


// #include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HttpClient.h>
#include <set>

#include "JSONManager.hpp"
#include "RTCManager.hpp"
#include "FlashManager.hpp"

#include "config.hpp"


typedef struct{
    const char* serverName;
    const char* path ; 
    uint16_t serverPort ;
    uint16_t successCode; 

} ServerInfo;





class HTTPManager{
    public:
        static void getRealTime() ;
        static void extractDateAndTimeFromAPIResponse();
        static void httpGetServer2Response();
        static void getProfile();
    private:
        static int16_t getStatusCode(ServerInfo *serverInf ,HttpClient *http);
        static void readHttpHeaders(HttpClient *http);
        static void getResponseString(String &response , HttpClient *http) ;


        static void extractTimeFromAPIResponse();
        static void extractDateFromAPIResponse();


};