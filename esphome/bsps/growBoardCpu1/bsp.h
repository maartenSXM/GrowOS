#ifndef _INCLUDE_BSP_H_
#define _INCLUDE_BSP_H_

#include "gos/gos.h"

#define GOS_BSP_HAS_psram
#define GOS_BSP_HAS_spi
#define GOS_BSP_HAS_i2c
#define GOS_BSP_HAS_sensor_internal_temperature
#define GOS_BSP_HAS_wifi

#define GOS_BSP_PIN_S_0			0
#define GOS_BSP_PIN_ACT_AUX		1
#define GOS_BSP_PIN_ACT_PUMP		2
#define GOS_BSP_PIN_ACT_LED		3
#define GOS_BSP_PIN_STA_D_EN		4
#define GOS_BSP_PIN_STA_C_EN		5
#define GOS_BSP_PIN_STA_B_EN		6
#define GOS_BSP_PIN_STA_A_EN		7
#define GOS_BSP_PIN_STA_E_EN		8
#define GOS_BSP_PIN_ACT_AIR		9
#define GOS_BSP_PIN_ACT_12V		10
#define GOS_BSP_PIN_FanPWM		11
#define GOS_BSP_PIN_HS_AUX		12
#define GOS_BSP_PIN_LOWWATER		13
#define GOS_BSP_PIN_DLOCK_GND_EN	14
#define GOS_BSP_PIN_KP_R1_OUT		15
#define GOS_BSP_PIN_KP_R2_OUT		16
#define GOS_BSP_PIN_KP_R3_OUT		17
#define GOS_BSP_PIN_KP_R4_OUT		18
#define GOS_BSP_PIN_KP_C1_OUT		19
#define GOS_BSP_PIN_KP_C2_OUT		20
#define GOS_BSP_PIN_KP_C3_OUT		21
#define GOS_BSP_PIN_SDnMS2_G		33
#define GOS_BSP_PIN_A_SD1_EN		34
#define GOS_BSP_PIN_SDnMS1_G		35
#define GOS_BSP_PIN_SDnMS3_G		36
#define GOS_BSP_PIN_SDnRst_G		37
#define GOS_BSP_PIN_A_SD1Sleep		38
#define GOS_BSP_PIN_SDnDIR_G		39
#define GOS_BSP_PIN_SDnStep_G		40
#define GOS_BSP_PIN_DOOR_POS		41
#define GOS_BSP_PIN_SET_MOTOR_MINUS	42
#define GOS_BSP_PIN_S_U0TXD		43
#define GOS_BSP_PIN_S_U0RXD		44
#define GOS_BSP_PIN_SET_MOTOR_PLUS	45

#endif // _INCLUDE_BSP_H_
