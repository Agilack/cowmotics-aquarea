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
#include "aquarea_ll.h"

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
	uint8_t req[256];

	printf("--=={ Cowmotics-Aquarea }==--\n");

	aquarea_ll_init();

	/* Get the current time (will be used to compute delay) */
	time(&tm_ref);

	while(1)
	{
		aquarea_ll_process();

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
			uart_write_bytes(AQUAREA_UART, (const char *)req, 111);

			/* Save a new time ref */
			tm_ref = tm_now;
		}

		/* Small delay for RTOS (and WDT !) */
		vTaskDelay(1);
	}
}
/* EOF */
