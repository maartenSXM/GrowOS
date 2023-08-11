#include "nvs_flash.h"
#include "nvs.h"
#include "flashStorage.h"

void saveFlashSettings()
{
  if (!enSaveToFlash)
    return;

  nvs_handle_t nvs_handle;
  esp_err_t ret = nvs_open("settings", NVS_READWRITE, &nvs_handle);
  if (ret != ESP_OK)
  {
    // Handle error
    return;
  }

  // sunrise hour
  nvs_set_i32(nvs_handle, "sunriseHour", sunriseTime.hour);

  // hours on
  nvs_set_u8(nvs_handle, "daylightHours", daylight);

  // hours off
  nvs_set_u8(nvs_handle, "nightHours", night);

  // day and week of grow
  nvs_set_u8(nvs_handle, "currentGrowDay", growDay);

  nvs_set_u8(nvs_handle, "currentGrowWeek", growWeek);

  // set fan temp
  nvs_set_i16(nvs_handle, "targetTemp", targetTemp);

  // save fan speed
  nvs_set_u16(nvs_handle, "fanSpeed", fanSpeed);

  nvs_close(nvs_handle);
}

void loadFlashSettings()
{
  return; // TODO: remove this line when flash is working
  // Open the NVS handle
  nvs_handle_t nvs_handle;
  esp_err_t ret = nvs_open("settings", NVS_READONLY, &nvs_handle);
  if (ret != ESP_OK)
  {
    // Handle error
    return;
  }

  // sunrise hour
  nvs_get_i32(nvs_handle, "sunriseHour", &sunriseTime.hour);

  // hours on
  nvs_get_u8(nvs_handle, "daylightHours", &daylight);

  // hours off
  nvs_get_u8(nvs_handle, "nightHours", &night);

  // set fan temp
  nvs_get_i16(nvs_handle, "targetTemp", &targetTemp);

  // day and week of grow
  nvs_get_u8(nvs_handle, "currentGrowDay", &growDay);
  if (growDay == 255)
    growDay = 1;

  nvs_get_u8(nvs_handle, "currentGrowWeek", &growWeek);
  if (growWeek == 255)
    growWeek = 1;

  // fan speed
  nvs_get_u16(nvs_handle, "fanSpeed", &fanSpeed);

  // CLOSE the NVS handle
  nvs_close(nvs_handle);
}