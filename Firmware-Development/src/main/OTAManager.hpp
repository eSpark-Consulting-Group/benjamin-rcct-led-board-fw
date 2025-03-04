#pragma once


#include <WiFi.h>
#include <AnchorOTA.h>

#define OTA_PORT 8082






class OTAManager{
    public:
        static void init();
    

    private:
        static void  otaThread(const void *);

        



};