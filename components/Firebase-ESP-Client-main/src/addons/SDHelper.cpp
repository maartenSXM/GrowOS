#include "SDHelper.h"

bool SD_Card_Mounting()
{

#if defined(DEFAULT_SD_FS) && defined(CARD_TYPE_SD)

    Serial.print("\nMounting SD Card... ");

#if defined(USE_SD_FAT_ESP32)

    if (!Firebase.sdBegin(&sdFatSPIConfig, SPI_CS_PIN, SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN)) //pointer to SdSpiConfig, SS, SCK,MISO, MOSI

#elif defined(ESP32) //if ESP32 and no SdFat library installed

    spi.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SPI_CS_PIN); //SPI pins config -> SCK,MISO, MOSI, SS
    if (!Firebase.sdBegin(SPI_CS_PIN, &spi)) //SS, pointer to SPIClass <- SPIClass object should defined as static or global

#elif defined(ESP8266)

    if (!Firebase.sdBegin(SPI_CS_PIN)) //or Firebase.sdBegin(&sdFSConfig)

#endif
    {
        Serial.println("failed\n");
        return false;
    }
    else
    {
        Serial.println("success\n");
        return true;
    }
#endif

#if defined(DEFAULT_SD_FS) && defined(CARD_TYPE_SD_MMC)

    Serial.print("\nMounting SD_MMC Card... ");

    if (!Firebase.sdMMCBegin("/sdcard", false, true))
    {
        Serial.println("failed\n");
        return false;
    }
    else
    {
        Serial.println("success\n");
        return true;
    }
#endif

    return false;
}