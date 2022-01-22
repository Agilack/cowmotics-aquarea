/**
 * @file  main/aquarea_ll.c
 * @brief Low level functions used to communicate with Aquarea (using UART)
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
#include <stdint.h>
#include <string.h>
#include "esp_err.h"
#include "aquarea_ll.h"

/* UART connected to Aquarea */
#define UART_TXD  17
#define UART_RXD  16
#define UART_RTS  UART_PIN_NO_CHANGE
#define UART_CTS  UART_PIN_NO_CHANGE
#define UART_BUF  1024

#define BUFFER_SIZE 258 /* Larger packet is 255 + 3 bytes */
uint16_t buffer_w;
uint8_t  buffer_rx[BUFFER_SIZE];

/**
 * @brief Initialize the Aquarea low-level module
 *
 * This function initialize internal variables and the UART used to communicate
 * with Aquarea. This function must be called before any other function of this
 * module.
 */
void aquarea_ll_init(void)
{
	buffer_w = 0;
	memset(buffer_rx, 0, BUFFER_SIZE);

	uart_config_t uart_config = {
		.baud_rate  = 9600,
		.data_bits  = UART_DATA_8_BITS,
		.parity     = UART_PARITY_EVEN,
		.stop_bits  = UART_STOP_BITS_1,
		.flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB,
	};

	/* Initialize UART connected to Aquarea */
	if (uart_driver_install(AQUAREA_UART, UART_BUF*2, 0, 0, NULL, 0) != ESP_OK)
		goto init_fail;
	if (uart_param_config(AQUAREA_UART, &uart_config) != ESP_OK)
		goto init_fail;
	if (uart_set_pin(AQUAREA_UART, UART_TXD, UART_RXD, UART_RTS, UART_CTS) != ESP_OK)
		goto init_fail;

	/* Success \o/ */
	return;

init_fail:
	printf("AQUAREA: Failed to init interface\r\n");
	// TODO Do something to clear this error or restart
}

/**
 * @brief Do necessary stuff for Aquarea communication
 *
 * This function must be called periodically to process data received from
 * aquarea and do necessary stuff.
 */
void aquarea_ll_process(void)
{
	uint8_t *pbuf;
	size_t   avail_sz, pkt_sz, rem_sz, len;
	int      i;

	/* Test is some data has been received from remote */
	if (uart_get_buffered_data_len(AQUAREA_UART, &avail_sz) != ESP_OK)
		goto err_uart;
	/* No data received ? nothing more to do here ;) */
	if (avail_sz <= 0)
		return;

	printf("Received %d bytes\r\n", (int)avail_sz);
	while(avail_sz)
	{
		/* First, wait for the 4-bytes header (with packet length) */
		if (buffer_w < 4)
		{
			len = (4 - buffer_w);
			pbuf = (buffer_rx + buffer_w);
			pkt_sz = 0xFFFF;
		}
		/* Then, read up to the specified length */
		else
		{
			/* Compute length of the full packet */
			pkt_sz = ((uint8_t)buffer_rx[1] + 3);
			/* Number of remaining bytes to have the full packet */
			rem_sz = pkt_sz - buffer_w;
			/* If there is more than one packet into rx */
			if (avail_sz > rem_sz)
				len = rem_sz;
			else
				len = avail_sz;

			if (pkt_sz < BUFFER_SIZE)
				pbuf = (buffer_rx + buffer_w);
			else
			{
				printf("AQUAREA: Error, packet larger than buffer (%d > %d)\n",
				       (unsigned int)pkt_sz, BUFFER_SIZE);
				pbuf = buffer_rx + 4;
				if (len > (BUFFER_SIZE - 4))
					len = (BUFFER_SIZE - 4);
			}
		}

		/* Read received data ! */
		len = uart_read_bytes(AQUAREA_UART, pbuf, len, 100);

		printf("Recv:");
		for (i = 0; i < len; i++)
			printf(" %.2X", pbuf[i]);
		printf("\r\n");

		/* Update counters */
		buffer_w += len;
		avail_sz -= len;
		if (buffer_w == pkt_sz)
		{
			printf("Packet fully received\n");
			buffer_w = 0;
		}
	}

	return;

err_uart:
	printf("AQUAREA: Major error into process()\r\n");
	return;
}
/* EOF */
