#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "pins.h"

#define PIN_KEEP -1

void espIdfUart0disable()
{
    ESP_ERROR_CHECK
	 (
	 uart_set_pin(0, _PIN_CSI_D2, _PIN_CSI_D3, PIN_KEEP, PIN_KEEP)
	 );
}
void espIdfUart0enable()
{
    ESP_ERROR_CHECK
	 (
	 uart_set_pin(0, _PIN_H_U0TXD, _PIN_H_U0RXD, PIN_KEEP, PIN_KEEP)
	 );
}

