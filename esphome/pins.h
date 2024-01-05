#ifndef _INCLUDE_CPU_H_
#define _INCLUDE_CPU_H_

#if _CPU == 0
#define _PIN_H_0		0
#define _PIN_CSI_VSYNC		1
#define _PIN_CSI_D9		2
#define _PIN_CSI_D8		3
#define _PIN_CSI_D7		4
#define _PIN_CSI_D6		5
#define _PIN_CTP_IRQ		6
#define _PIN_CTP_RST		7
#define _PIN_TFT_RST		7
#define _PIN_TWI_SDA		8
#define _PIN_TWI_SCK		9
#define _PIN_CSI_PWR_EN		10
#define _PIN_CSI_PCLK		11
#define _PIN_CSI_D4		12
#define _PIN_H_S_ENABLE		13
#define _PIN_PWROFF_INT		14
#define _PIN_XCTL_HIGH		15
#define _PIN_XCTL_LOW		16
#define _PIN_TempData		17
#define _PIN_TFT_PWR_EN		18
#define _PIN_H_USB_NEG		19
#define _PIN_H_USB_POS		20
#define _PIN_TFT_BACKLIGHT 	21
#define _PIN_MYSTERY		26
#define _PIN_TFT_CS		33
#define _PIN_CSI_HREF		33
#define _PIN_TFT_DC		34
#define _PIN_CSI_XCLK		34
#define _PIN_TFT_MOSI		35
#define _PIN_CSI_D2		35
#define _PIN_TFT_SCLK		36
#define _PIN_CSI_D3		36
#define _PIN_TFT_MISO		37
#define _PIN_CSI_D5		37
#define _PIN_VBAT_SENSE		38
#define _PIN_H_39		39
#define _PIN_H_40		40
#define _PIN_H_41		41
#define _PIN_H_42		42
#define _PIN_BUZZER		42
#define _PIN_H_U0TXD		43
#define _PIN_H_U0RXD		44
#define _PIN_VBAT_CE		45

#else // _CPU == 1
 
#define _PIN_S_0		0
#define _PIN_ACT_AUX		1
#define _PIN_ACT_PUMP		2
#define _PIN_ACT_LED		3
#define _PIN_STA_D_EN		4
#define _PIN_STA_C_EN		5
#define _PIN_STA_B_EN		6
#define _PIN_STA_A_EN		7
#define _PIN_STA_E_EN		8
#define _PIN_ACT_AIR		9
#define _PIN_ACT_12V		10
#define _PIN_FanPWM		11
#define _PIN_HS_AUX		12
#define _PIN_LOWWATER		13
#define _PIN_DLOCK_GND_EN	14
#define _PIN_KP_R1_OUT		15
#define _PIN_KP_R2_OUT		16
#define _PIN_KP_R3_OUT		17
#define _PIN_KP_R4_OUT		18
#define _PIN_KP_C1_OUT		19
#define _PIN_KP_C2_OUT		20
#define _PIN_KP_C3_OUT		21
#define _PIN_SDnMS2_G		33
#define _PIN_A_SD1_EN		34
#define _PIN_SDnMS1_G		35
#define _PIN_SDnMS3_G		36
#define _PIN_SDnRst_G		37
#define _PIN_A_SD1Sleep		38
#define _PIN_SDnDIR_G		39
#define _PIN_SDnStep_G		40
#define _PIN_DOOR_POS		41
#define _PIN_SET_MOTOR_MINUS	42
#define _PIN_S_U0TXD		43
#define _PIN_S_U0RXD		44
#define _PIN_SET_MOTOR_PLUS	45

#endif // _CPU == 1

#endif // _INCLUDE_CPU_H_
