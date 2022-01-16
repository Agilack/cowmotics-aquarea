/**
 * @file  main/main.c
 * @brief Main function of the "main" component
 *
 * @author Saint-Genest Gwenael <gwen@agilack.fr>
 * @copyright Agilack (c) 2022
 *
 * @page License
 * This firmware is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published
 * by the Free Software Foundation. You should have received a copy of the
 * GNU General Public License along with this program, see LICENSE.md file
 * for more details.
 * This program is distributed WITHOUT ANY WARRANTY.
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* UART connected to Aquarea */
#define UART_PORT UART_NUM_2
#define UART_TXD  17
#define UART_RXD  16
#define UART_RTS  UART_PIN_NO_CHANGE
#define UART_CTS  UART_PIN_NO_CHANGE

#define BUF_SIZE  1024

/**
 * @brief Entry point of the main task
 *
 * The main task is created by esp-idf after low-level and FreeRTOS
 * initialization. The special function "app_main" is then called into
 * the main task context.
 * For the moment, initialization and main loop of the firmware are
 * into the function/task.
 */
void app_main(void)
{
	time_t  tm_ref, tm_now;
	size_t  len;
	uint8_t data[64];
	uint8_t req[256];
	int i;

	printf("--=={ Cowmotics-Aquarea }==--\n");

	uart_config_t uart_config = {
		.baud_rate  = 9600,
		.data_bits  = UART_DATA_8_BITS,
		.parity     = UART_PARITY_EVEN,
		.stop_bits  = UART_STOP_BITS_1,
		.flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB,
	};
	/* Initialize UART connected to Aquarea */
	ESP_ERROR_CHECK( uart_driver_install(UART_PORT, BUF_SIZE*2, 0, 0, NULL, 0) );
	ESP_ERROR_CHECK(   uart_param_config(UART_PORT, &uart_config) );
	ESP_ERROR_CHECK(        uart_set_pin(UART_PORT, UART_TXD, UART_RXD, UART_RTS, UART_CTS) );

	/* Get the current time (will be used to compute delay) */
	time(&tm_ref);

	while(1)
	{
		/* Test is some data has been received from remote ... */
		ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_PORT, &len));
		/* ... Yes ! Read and process them */
		if (len > 0)
		{
			printf("Received %d bytes\r\n", len);
			if (len > 16)
				len = 16;
			len = uart_read_bytes(UART_PORT, data, len, 100);
			printf("Recv:");
			for (i = 0; i < len; i++)
				printf(" %.2X", data[i]);
			printf("\r\n");
		}

		/* If 5 sec elapsed since last query, query again */
		time(&tm_now);
		if ((tm_now - tm_ref) > 5)
		{
			printf("\r\nSend a query packet ...\r\n");

			memset(req, 0, 256);
			/* Insert request header */
			req[0] = 0x71;
			req[1] = 108;  // Payload length
			req[2] = 0x01;
			req[3] = 0x10;
			/* Insert checksum */
			req[110] = 0x12; // TODO use computed value
			/* Send request to Aquarea */
			uart_write_bytes(UART_PORT, (const char *)req, 111);

			/* Save a new time ref */
			tm_ref = tm_now;
		}

		/* Small delay for RTOS (and WDT !) */
		vTaskDelay(1);
	}
}
/* EOF */
