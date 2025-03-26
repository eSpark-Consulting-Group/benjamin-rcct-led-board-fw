#pragma once



#include "httpOTA.hpp"
#include "config.hpp"


class OTAManager{

    public:

        void beginHttpOTA();

    private:

        const char *otaServerName= OTA_SERVER_DOMAIN_NAME;
        const char *otaServerPath= OTA_SERVER_PATH;
        uint16_t otaServerPort= OTA_SERVER_PORT;

};

struct {
    String version;
    String url;
}ota;

extern OTAManager otaManage;