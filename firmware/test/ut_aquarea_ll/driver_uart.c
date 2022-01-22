/**
 * @file  driver_uart.c
 * @brief Simulate esp-idf uart driver to inject data into tested component
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
#include "driver/uart.h"
#include "esp_err.h"

unsigned char *buffer;
unsigned int   buffer_len;

int uart_driver_install(int uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, void *uart_queue, int intr_alloc_flags)
{
	return ESP_OK;
}

int uart_get_buffered_data_len(int uart_num, size_t* size)
{
	if (size != 0)
		*size = buffer_len;
	return(0);
}

int uart_param_config(int uart_num, const uart_config_t *uart_config)
{
	return(ESP_OK);
}

int uart_read_bytes(int uart_num, void* buf, uint32_t length, int ticks_to_wait)
{
	if (length > buffer_len)
		length = buffer_len;

	memcpy((unsigned char *)buf, buffer, length);
	buffer += length;

	buffer_len -= length;
	
	return(length);
}

int uart_set_pin(int uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num)
{
	return(ESP_OK);
}

/* -------------------- */
void uart_init(void)
{
	buffer_len = 0;
	buffer = 0;
}
void uart_set_buffer(unsigned char *src, int len)
{
	printf("DRV: Insert %d bytes into RX buffer\n", len);
	buffer = src;
	buffer_len = len;
}
/* EOF */
