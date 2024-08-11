#ifndef _INCLUDE_BSP_H_
#define _INCLUDE_BSP_H_

#include "gos/gos.h"

#define GOS_BSP_HAS_esp32_ble
#define GOS_BSP_HAS_psram
#define GOS_BSP_HAS_spi
#define GOS_BSP_HAS_i2c
#define GOS_BSP_HAS_touchscreen
#define GOS_BSP_HAS_display
#define GOS_BSP_HAS_sensor_internal_temperature
#define GOS_BSP_HAS_wifi

#define GOS_BSP_HAS_FLASH_16MB

// GOS_BSP_PIN_ suffices are the pin labels from the PDF schematic here:
//   github.com/Xinyuan-LilyGO/LilyGo-AMOLED-Series/blob/master/schematic/T4-S3-AMOLED.pdf

#define GOS_BSP_PIN_SD_CS		1
#define GOS_BSP_PIN_SD_MOSI		2
#define GOS_BSP_PIN_SD_SCK		3
#define GOS_BSP_PIN_SD_MISO		4
#define GOS_BSP_PIN_6970_INT		5
#define GOS_BSP_PIN_II2C_SDA		6
#define GOS_BSP_PIN_II2C_SCL		7
#define GOS_BSP_PIN_TP_INT		8
#define GOS_BSP_PIN_V_EN		9
#define GOS_BSP_PIN_VDD_EV		9
#define GOS_BSP_PIN_SPI_SI1		10
#define GOS_BSP_PIN_SPI_CS		11
#define GOS_BSP_PIN_SPI_SI3		12
#define GOS_BSP_PIN_SPI_RST		13
#define GOS_BSP_PIN_SPI_SI0		14
#define GOS_BSP_PIN_SPI_CLK		15
#define GOS_BSP_PIN_SPI_SI2		16
#define GOS_BSP_PIN_XTAL_32K_P		15
#define GOS_BSP_PIN_XTAL_32K_N		16
#define GOS_BSP_PIN_TP_RST		17
#define GOS_BSP_PIN_D_N			19
#define GOS_BSP_PIN_D_P			20
#define GOS_BSP_PIN_IO_MUX_DAC_1	17
#define GOS_BSP_PIN_IO_MUX_DAC_2	18
#define GOS_BSP_PIN_SPICS1		26
#define GOS_BSP_PIN_SPIHD		27
#define GOS_BSP_PIN_SPIWP		28
#define GOS_BSP_PIN_SPICS0		29
#define GOS_BSP_PIN_SPICLK		30
#define GOS_BSP_PIN_SPIQ		31
#define GOS_BSP_PIN_SPID		32
#define GOS_BSP_PIN_MTCK		39
#define GOS_BSP_PIN_MTDO		40
#define GOS_BSP_PIN_MTDI		41
#define GOS_BSP_PIN_MTMS		42
#define GOS_BSP_PIN_U0TXD		43
#define GOS_BSP_PIN_U0RXD		44
#define GOS_BSP_PIN_SPICLK_P		47
#define GOS_BSP_PIN_SPICLK_N		48

#endif // _INCLUDE_BSP_H_
