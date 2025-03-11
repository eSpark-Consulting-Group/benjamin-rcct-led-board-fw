#include "FlashManager.hpp"





void FlashManager::init(){
    FlashMemory.begin(FLASH_MEMORY_BASE, FLASH_INIT_SIZE);
    FlashMemory.read();
}


void FlashManager::save_wifi_creds(String ssid , String pass){
      memset(FlashMemory.buf, 0, MEMSET_BUFF_LEN_WIFICREDS);
      strcpy((char *)(FlashMemory.buf + FLASH_SSID_OFFSET), ssid.c_str());
      strcpy((char *)(FlashMemory.buf + FLASH_PASS_OFFSET), pass.c_str());

      FlashMemory.update();
      Serial.println("Wi-Fi credentials saved.");


}


void FlashManager::load_wifi_creds(String &ssid, String &pass) {
      Serial.println("Loading Wi-Fi credentials from flash...");
  
      char ssidBuf[SSID_BUFF_LEN] = {0};  // Zero-initialize buffers
      char passBuf[PASS_BUFF_LEN] = {0};
  
      // Copy only the allowed buffer size
      memcpy(ssidBuf, FlashMemory.buf + FLASH_SSID_OFFSET, SSID_BUFF_LEN - 1);
      memcpy(passBuf, FlashMemory.buf + FLASH_PASS_OFFSET, PASS_BUFF_LEN - 1);
  
      // Manually add null terminator to prevent out-of-bounds reads
      ssidBuf[SSID_BUFF_LEN - 1] = '\0';
      passBuf[PASS_BUFF_LEN - 1] = '\0';
  
      // Check if credentials are valid (not empty or erased flash values)
      if (ssidBuf[0] == 0 || ssidBuf[0] == 0xFF) {
          Serial.println("No valid SSID found in flash. Waiting for BLE credentials...");
          return;
      }
      if (passBuf[0] == 0 || passBuf[0] == 0xFF) {
          Serial.println("No valid password found in flash. Waiting for BLE credentials...");
          return;
      }
  
      // Convert to String
      ssid = String(ssidBuf);
      pass = String(passBuf);
  }



uint32_t FlashManager::readWord(uint32_t offsetAddress){
    uint32_t storedValue = FlashMemory.readWord(offsetAddress);
    return(storedValue);

}

void FlashManager::writeWord( uint32_t buffer , uint32_t address ){
    FlashMemory.writeWord(address, buffer);
}


void FlashManager::verifyData( uint32_t offsetAddress, uint32_t elements){

    for (uint32_t i = 0; i < elements; i++) {  // Divide byteCount by 4 to get the number of words
        uint32_t storedValue = FlashMemory.readWord(offsetAddress);
        Serial.print(storedValue);
        Serial.print(",");
        
        offsetAddress += WORD_SIZE;  // Move to the next word
    }


}