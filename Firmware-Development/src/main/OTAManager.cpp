// #include "OTAManager.hpp"

// #define OTA_PORT 80



// IPAddress ip = {192,168,1,53};





// void OTAManager::init(){
//   os_thread_create_arduino(otaThread, NULL, OS_PRIORITY_REALTIME, 4096);

// }


// void  OTAManager::otaThread(const void *){
//     while(true){
//       if (OTA.beginCloud(ip, OTA_PORT) < 0) {
//       Serial.println("OTA failed!");
//       }
//       vTaskDelay(pdMS_TO_TICKS(2000));
//     }
//   }