#ifndef _INCLUDE_BSP_H_
#define _INCLUDE_BSP_H_

#include "gos/gos.h"

#define GOS_BSP_HAS_psram
#define GOS_BSP_HAS_spi
#define GOS_BSP_HAS_i2c
#define GOS_BSP_HAS_backlight
#define GOS_BSP_HAS_backlight_gpio
#define GOS_BSP_HAS_touchscreen
#define GOS_BSP_HAS_touchscreen_ft63x6
#define GOS_BSP_HAS_touchscreen_platform ft63x6
#define GOS_BSP_HAS_display
#define GOS_BSP_HAS_display_platform ili9xxx
#define GOS_BSP_HAS_display_ili9xxx
#define GOS_BSP_HAS_output_pwm_led
#define GOS_BSP_HAS_output_buzzer
#define GOS_BSP_HAS_esp32_camera
#define GOS_BSP_HAS_sensor_internal_temperature
#define GOS_BSP_HAS_uart
// #define GOS_BSP_HAS_usb

#define GOS_BSP_HAS_32_KHZ_XTAL
#define GOS_BSP_HAS_FLASH_4MB

#define GOS_BSP_PIN_H_0			0
#define GOS_BSP_PIN_CSI_VSYNC		1
#define GOS_BSP_PIN_CSI_D9		2
#define GOS_BSP_PIN_CSI_D8		3
#define GOS_BSP_PIN_CSI_D7		4
#define GOS_BSP_PIN_CSI_D6		5
#define GOS_BSP_PIN_CTP_IRQ		6
#define GOS_BSP_PIN_CTP_RST		7
#define GOS_BSP_PIN_TFT_RST		7
#define GOS_BSP_PIN_TWI_SDA		8
#define GOS_BSP_PIN_TWI_SCK		9
#define GOS_BSP_PIN_CSI_PWR_EN		10
#define GOS_BSP_PIN_CSI_PCLK		11
#define GOS_BSP_PIN_CSI_D4		12
#define GOS_BSP_PIN_H_S_ENABLE		13
#define GOS_BSP_PIN_PWROFF_INT		14
#define GOS_BSP_PIN_XCTL_HIGH		15
#define GOS_BSP_PIN_XCTL_LOW		16
#define GOS_BSP_PIN_TempData		17
#define GOS_BSP_PIN_TFT_PWR_EN		18
#define GOS_BSP_PIN_H_USB_NEG		19
#define GOS_BSP_PIN_H_USB_POS		20
#define GOS_BSP_PIN_TFT_BACKLIGHT 	21
#define GOS_BSP_PIN_MYSTERY		26
#define GOS_BSP_PIN_TFT_CS		33
#define GOS_BSP_PIN_CSI_HREF		33
#define GOS_BSP_PIN_TFT_DC		34
#define GOS_BSP_PIN_CSI_XCLK		34
#define GOS_BSP_PIN_TFT_MOSI		35
#define GOS_BSP_PIN_CSI_D2		35
#define GOS_BSP_PIN_TFT_SCLK		36
#define GOS_BSP_PIN_CSI_D3		36
#define GOS_BSP_PIN_TFT_MISO		37
#define GOS_BSP_PIN_CSI_D5		37
#define GOS_BSP_PIN_VBAT_SENSE		38
#define GOS_BSP_PIN_H_39		39
#define GOS_BSP_PIN_H_40		40
#define GOS_BSP_PIN_H_41		41
#define GOS_BSP_PIN_H_42		42
#define GOS_BSP_PIN_BUZZER		42
#define GOS_BSP_PIN_H_U0TXD		43
#define GOS_BSP_PIN_H_U0RXD		44
#define GOS_BSP_PIN_VBAT_CE		45

#endif // _INCLUDE_BSP_H_
