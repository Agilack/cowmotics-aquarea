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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "aquarea.h"

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
	printf("--=={ Cowmotics-Aquarea }==--\n");

	aquarea_init();

	while(1)
	{
		aquarea_process();

		/* Small delay for RTOS (and WDT !) */
		vTaskDelay(1);
	}
}
/* EOF */
