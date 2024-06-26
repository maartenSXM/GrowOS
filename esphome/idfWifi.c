#include "esp_wifi.h"

int idfWifiGetChannelNum(void)
    {
    wifi_ap_record_t ap_info;

    if (esp_wifi_sta_get_ap_info (&ap_info) != ESP_OK)
	return (-1);

    return (ap_info.primary);
    }
