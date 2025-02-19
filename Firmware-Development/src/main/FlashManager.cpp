#include "FlashManager.hpp"



void FlashManager::save_wifi_creds(String ssid , String pass){
      memset(FlashMemory.buf, 0, 128);
      strcpy((char *)(FlashMemory.buf + FLASH_SSID_OFFSET), ssid.c_str());
      strcpy((char *)(FlashMemory.buf + FLASH_PASS_OFFSET), pass.c_str());

      FlashMemory.update();
      Serial.println("Wi-Fi credentials saved.");


}

void FlashManager::load_wifi_creds(String &ssid , String &pass){
      Serial.println("Loading Wi-Fi credentials from flash...");

      char ssidBuf[64];
      char passBuf[64];

      strcpy(ssidBuf, (char *)(FlashMemory.buf + FLASH_SSID_OFFSET));
      strcpy(passBuf, (char *)(FlashMemory.buf + FLASH_PASS_OFFSET));

      if (strlen(ssidBuf) > 0 && strlen(passBuf) > 0) {
          ssid = String(ssidBuf);
          pass = String(passBuf);
      }
}


// void FlashManager::save_single_input_vals(String col , uint16_t temp , uint16_t bright){
//     // Clear only the color storage area (optional)
//     memset(FlashMemory.buf + FLASH_COLOR_OFFSET, 0, 64);  

//     // Ensure the string fits within the allocated space
//     strncpy((char *)(FlashMemory.buf + FLASH_COLOR_OFFSET), col.c_str(), 63);  
//     FlashMemory.buf[FLASH_COLOR_OFFSET + 63] = '\0';  // Null-terminate the string

//     // Store numerical values safely
//     memcpy(FlashMemory.buf + FLASH_TEMP_OFFSET, &temp, sizeof(temp));
//     memcpy(FlashMemory.buf + FLASH_BRIGHT_OFFSET, &bright, sizeof(bright));

//     FlashMemory.update();  // Commit to non-volatile memory

//     Serial.println("Single input values saved.");
// }

// void FlashManager::load_single_input_vals(String &col, uint16_t &temp, uint16_t &bright) {
//     // Read color string safely
//     char color[64] = {0};  
//     strncpy(color, (char *)(FlashMemory.buf + FLASH_COLOR_OFFSET), 63);
//     col = String(color);

//     // Read numerical values
//     memcpy(&temp, FlashMemory.buf + FLASH_TEMP_OFFSET, sizeof(temp));
//     memcpy(&bright, FlashMemory.buf + FLASH_BRIGHT_OFFSET, sizeof(bright));

//     Serial.println("Single input values loaded:");
//     Serial.print("Color: "); Serial.println(col);
//     Serial.print("Temperature: "); Serial.println(temp);
//     Serial.print("Brightness: "); Serial.println(bright);
// }



