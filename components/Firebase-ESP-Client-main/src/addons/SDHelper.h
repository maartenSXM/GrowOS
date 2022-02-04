
#ifndef SD_HELPER_H_
#define SD_HELPER_H_

#include <Arduino.h>
#include "Firebase.h"

//If SD Card used for storage, assign SD card type and FS used in src/FirebaseFS.h and
//change the config for that card interfaces in this file (src/addons/SDHelper.h)

#if defined(DEFAULT_SD_FS) && defined(CARD_TYPE_SD)

#if defined(ESP32)
#define SPI_CS_PIN 13
#define SPI_SCK_PIN 14
#define SPI_MISO_PIN 2
#define SPI_MOSI_PIN 15
#define SPI_CLOCK_IN_MHz 16
#elif defined(ESP8266)
#define SPI_CS_PIN 15
#endif

//if SdFat library installed and FirebaseFS.h was set to use it (for ESP32 only)
#if defined(USE_SD_FAT_ESP32)

//https://github.com/greiman/SdFat
SdSpiConfig sdFatSPIConfig(SPI_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(SPI_CLOCK_IN_MHz));
bool SD_Card_Mounting();
#elif defined(ESP32) //if ESP32 and no SdFat library installed

SPIClass spi;

#elif defined(ESP8266)

//SDFSConfig sdFSConfig(SPI_CS_PIN, SPI_HALF_SPEED);

#endif

#endif

#endif  //SD_HELPER_H_
