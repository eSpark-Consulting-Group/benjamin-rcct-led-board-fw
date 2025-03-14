#include "OTAManager.hpp"


const char otaServerName[]= "184.169.181.179";
const char otaServerPath[]= "/api/ota";
uint16_t otaServerPort= 3004;




void OTAManager::beginHttpOTA(){
    

    OTA.beginCloud(otaServerName , otaServerPath ,otaServerPort);

}