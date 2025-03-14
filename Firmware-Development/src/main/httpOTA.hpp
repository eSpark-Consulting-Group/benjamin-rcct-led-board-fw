#pragma once

#include "Arduino.h"
#include "flash_api.h"
#include "ard_socket.h"
#include "sys_api.h"
#include "mDNS.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <HttpClient.h>
#include <set>

#define OTA_ADDRESS_1 0x6000
#define OTA_ADDRESS_2 0x106000

#define OTA_VALID 0x35393138
#define OTA_VALID2 0x31313738
#define OTA_INVALID 0x35393130 //anything that's not OTA_VALID

#define BUFSIZE 512
#define DEFAULT_OTA_MDNS_BUF 128
#define IMAGE_SECTOR_SIZE 4096
#define DEFAULT_IMAGE_DOWNLOAD_TIMEOUT 30000

class OTAClass {

public:
    OTAClass();
    
    int beginCloud(const char* otaServer, const char* otaServerPath ,uint16_t otaServerPort ,bool rebootOnSuccess =true) ;

private:
    int receiveImage(int imageLength);
    int receiveImageHeader( int& imageLength, int& imageChecksum);
    int calculateImageChecksum(int& calculatedChecksum, int imageLength);
    int eraseImageFlash(int imageLength);
    int choseOtaAddress();
    int createImageHeader();

    uint32_t otaAddrOld;
    uint32_t otaAddrNew;

    flash_t flash;
    unsigned char *mdnsBuffer;
    void *mdnsServiceId;
    void *txtRecord;
};

extern OTAClass OTA;

