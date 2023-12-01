#include "esp32SystemAPIHandler.h"

void initialize_esp32_system_variables(){
  getESP32BaseMacAddr();
}

void getESP32BaseMacAddr(){
    esp_efuse_mac_get_default(ESP32_BASE_MAC_ADDRESS);
}

