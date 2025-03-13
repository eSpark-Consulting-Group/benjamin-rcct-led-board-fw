

#include <AnchorOTA.h>


#define OTA_PORT 80

// Address of device where this board can fetch its updates
IPAddress ip = {192,168,2,148};

void otaThread(const void *){
    while(true){
      if (OTA.beginCloud(ip, OTA_PORT) < 0) {
      Serial.println("OTA failed!");
      }
      vTaskDelay(pdMS_TO_TICKS(10000));
  
    }
  }

void otaStart() {
  os_thread_create_arduino(otaThread, NULL, OS_PRIORITY_REALTIME, 4096);
}



