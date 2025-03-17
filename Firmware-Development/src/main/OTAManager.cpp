#include "OTAManager.hpp"







void OTAManager::beginHttpOTA(){
    

    OTA.beginCloud(otaServerName , otaServerPath ,otaServerPort);

}

OTAManager otaManage;