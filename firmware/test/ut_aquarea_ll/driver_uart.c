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
#include "log.h"

unsigned char *buffer;
unsigned int   buffer_len;

static int init_drv, init_cfg, init_pin;
static int init_drv_force, init_cfg_force, init_pin_force;

/* -------------------------------------------------------------------------- */
/* --                       Config and Init function                       -- */
/* -------------------------------------------------------------------------- */

/**
 * @brief Simulated version of esp-idf function uart_driver_install
 *
 * @param uart_num Identifier of the UART port to init
 * @param rx_buffer_size Size (in bytes) of the RX fifo
 * @param tx_buffer_size Size (in bytes) of the TX fifo
 * @param queue_size Number of element into event queue (not used here)
 * @param uart_queue Pointer to the event queue (not used here)
 * @param intr_alloc_flags Flags for the UART interrupt (not used here)
 * @return integer ESP_OK is returned of success, else ESP_FAIL
 */
int uart_driver_install(uart_port_t uart_num,
                        int rx_buffer_size, int tx_buffer_size,
                        int queue_size, void *uart_queue, int intr_alloc_flags)
{
	int init_result = 1;
	int result = ESP_OK;

	/* Check UART number argument */
	if (uart_num != UART_NUM_2)
	{
		printf(COLOR_RED "INIT: uart_driver_install called for wrong UART : %d\n" COLOR_NONE, uart_num);
		init_result = -1;
	}
	/* Test if RX fifo is large enough for Aquarea packets */
	if (rx_buffer_size < (258 * 2))
	{
		printf(COLOR_RED "INIT: uart_driver_install() RX buffer size too small %d < %d\n" COLOR_NONE, rx_buffer_size, (258*2));
		init_result = -1;
	}
	init_drv = init_result;

	if (init_drv_force)
		result = ESP_FAIL;

	return(result);
}

/**
 * @brief Configure the UART (speed, format, ...)
 *
 * @param uart_num Identifier of the UART port to init
 * @param config Pointer to a structure with UART configuration
 * @return integer ESP_OK is returned of success, else ESP_FAIL
 */
int uart_param_config(uart_port_t uart_num, const uart_config_t *config)
{
	int init_result = 1;
	int result = ESP_OK;

	/* Check UART number argument */
	if (uart_num != UART_NUM_2)
	{
		printf(COLOR_RED "INIT: uart_param_config called for wrong UART : %d\n" COLOR_NONE, uart_num);
		init_result = -1;
	}
	/* Check specified port speed */
	if (config->baud_rate != 9600)
	{
		printf(COLOR_RED "INIT: uart_param_config Wrong speed specified : %d\n" COLOR_NONE, config->baud_rate);
		init_result = -1;
	}
	/* Check specified parity format */
	if (config->parity != UART_PARITY_EVEN)
	{
		printf(COLOR_RED "INIT: uart_param_config Wrong parity format : %d\n" COLOR_NONE, config->parity);
		init_result = -1;
	}

	init_cfg = init_result;

	if (init_cfg_force)
		result = ESP_FAIL;

	return(result);
}

/**
 * @brief Simulated version of esp-idf function uart_set_pin
 *
 * @param uart_num Identifier of the UART port to init
 * @param tx_io_num GPIO number to use for TX
 * @param rx_io_num GPIO number to use for RX
 * @param rts_io_num GPIO number to use for RTS
 * @param cts_io_num GPIO number to use for CTS
 * @return integer ESP_OK is returned of success, else ESP_FAIL
 */
int uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num,
                 int rts_io_num, int cts_io_num)
{
	int init_result = 1;
	int result = ESP_OK;

	/* Check UART number argument */
	if (uart_num != UART_NUM_2)
	{
		printf(COLOR_RED "INIT: uart_set_pin called for wrong UART : %d\n" COLOR_NONE, uart_num);
		init_result = -1;
	}
	/* Check TX pin */
	if (tx_io_num != 17)
	{
		printf(COLOR_RED "INIT: uart_set_pin Wrong TX pin : %d\n" COLOR_NONE, tx_io_num);
		init_result = -1;
	}
	/* Check RX pin */
	if (rx_io_num != 16)
	{
		printf(COLOR_RED "INIT: uart_set_pin Wrong RX pin : %d\n" COLOR_NONE, rx_io_num);
		init_result = -1;
	}
	/* Check RTS value (should not be defined) */
	if (rts_io_num != UART_PIN_NO_CHANGE)
	{
		printf(COLOR_RED "INIT: uart_set_pin Wrong RTS pin : %d\n" COLOR_NONE, rts_io_num);
		init_result = -1;
	}
	/* Check CTS value (should not be defined) */
	if (cts_io_num != UART_PIN_NO_CHANGE)
	{
		printf(COLOR_RED "INIT: uart_set_pin Wrong CTS pin : %d\n" COLOR_NONE, cts_io_num);
		init_result = -1;
	}
	init_pin = init_result;

	if (init_pin_force)
		result = ESP_FAIL;

	return(result);
}

/* -------------------------------------------------------------------------- */
/* --                          UART IOs functions                          -- */
/* -------------------------------------------------------------------------- */

int uart_get_buffered_data_len(int uart_num, size_t* size)
{
	if (size != 0)
		*size = buffer_len;
	return(0);
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

int uart_write_bytes(int uart_num, const void *src, int size)
{
	return(0);
}

/* -------------------------------------------------------------------------- */
/* --                       Internal tests functions                       -- */
/* -------------------------------------------------------------------------- */

void uart_init(void)
{
	init_drv = 0;
	init_drv_force = 0;
	init_cfg = 0;
	init_cfg_force = 0;
	init_pin = 0;
	init_pin_force = 0;

	buffer_len = 0;
	buffer = 0;
}

void uart_set_buffer(unsigned char *src, int len)
{
	printf("DRV: Insert %d bytes into RX buffer\n", len);
	buffer = src;
	buffer_len = len;
}

int uart_test_drv(int force)
{
	if (force == 1)
		init_drv_force = 1;
	else if (force == 2)
		init_drv_force = 0;

	return(init_drv);
}

int uart_test_cfg(int force)
{
	if (force == 1)
		init_cfg_force = 1;
	else if (force == 2)
		init_cfg_force = 0;

	return(init_cfg);
}

int uart_test_pin(int force)
{
	if (force == 1)
		init_pin_force = 1;
	else if (force == 2)
		init_pin_force = 0;

	return(init_pin);
}
/* EOF */
