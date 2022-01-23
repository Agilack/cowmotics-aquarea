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
#include "driver/uart.h"
#include "esp_err.h"
#include "aquarea_ll.h"

/* UART connected to Aquarea */
#if (AQUAREA_UART == 2)
#define UART_PORT UART_NUM_2
#endif
#define UART_TXD  17
#define UART_RXD  16
#define UART_RTS  UART_PIN_NO_CHANGE
#define UART_CTS  UART_PIN_NO_CHANGE
#define UART_BUF  1024

#define BUFFER_SIZE 258 /* Larger packet is 255 + 3 bytes */
uint16_t buffer_w;
uint8_t  buffer_rx[BUFFER_SIZE];

#ifdef AQUAREA_LOG
unsigned int aquarea_ll_log = 0;
#endif

/* Declare RX handler, must be implemented elsewhere */
void aquarea_rx(uint8_t *packet, size_t len);

/* Internal functions */
static uint8_t checksum(uint8_t *buffer, int len);
static int checksum_verify(uint8_t *packet);

/**
 * @brief Initialize the Aquarea low-level module
 *
 * This function initialize internal variables and the UART used to communicate
 * with Aquarea. This function must be called before any other function of this
 * module.
 */
int aquarea_ll_init(void)
{
#ifdef AQUAREA_LOG
	aquarea_ll_log = 0;
#endif
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
	return(0);

init_fail:
	printf("AQUAREA: Failed to init interface\r\n");
	// TODO Do something to clear this error or restart
	return(-1);
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
#ifdef AQUAREA_LOG
	int i;
#endif

	/* Test is some data has been received from remote */
	if (uart_get_buffered_data_len(AQUAREA_UART, &avail_sz) != ESP_OK)
		goto err_uart;
	/* No data received ? nothing more to do here ;) */
	if (avail_sz <= 0)
		return;

#ifdef AQUAREA_LOG
	if (aquarea_ll_log & (1 << 8))
		printf("Received %d bytes\r\n", (int)avail_sz);
#endif
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

			if (pkt_sz <= BUFFER_SIZE)
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
#ifdef AQUAREA_LOG
		if (aquarea_ll_log & (2 << 8))
		{
			printf("Recv:");
			for (i = 0; i < len; i++)
				printf(" %.2X", pbuf[i]);
			printf("\r\n");
		}
#endif
		/* Update counters */
		buffer_w += len;
		avail_sz -= len;

		/* If all byets of the packet have been received :) */
		if (buffer_w == pkt_sz)
		{
			if (checksum_verify(buffer_rx))
			{
				printf("Packet fully received\n");
				aquarea_rx(buffer_rx, pkt_sz);
			}
			else
				printf("AQUAREA: Ignore invalid packet\n");
			buffer_w = 0;
		}
	}

	return;

err_uart:
	printf("AQUAREA: Major error into process()\r\n");
	return;
}

/**
 * @brief Send a packet to Aquarea
 *
 * @param packet Pointer to a byte array with packet to send
 * @return integer Number of bytes sent
 */
int aquarea_ll_send(unsigned char *packet)
{
	size_t pkt_len;
#ifdef AQUAREA_LOG
	int i;
#endif

	/* Compute packet length */
	pkt_len = (packet[1] + 3);

	/* Insert packet checksum */
	packet[pkt_len - 1] = checksum(packet, pkt_len - 1);

	/* Send request to Aquarea */
	uart_write_bytes(AQUAREA_UART, (const char *)packet, pkt_len);

#ifdef AQUAREA_LOG
	printf("Send packet :\n");
	for (i = 0; i < pkt_len; i++)
	{
		printf("%.2X ", packet[i]);
		if ((i & 15) == 15)
			printf("\n");
	}
	if ((i & 15) != 0)
		printf("\n");
#endif

	return(pkt_len);
}

/* -------------------------------------------------------------------------- */
/* --                          Internal functions                          -- */
/* -------------------------------------------------------------------------- */

/**
 * @brief Compute the checksum of a buffer
 *
 * @param buffer Pointer to an array with input bytes
 * @param len    Number of bytes into the buffer
 * @return uint8 Value of the computed checksum
 */
static uint8_t checksum(uint8_t *buffer, int len)
{
	uint32_t cksum;
	int i;

	cksum = 0;
	for (i = 0; i < len; i++)
		cksum += buffer[i];
	cksum = ((cksum & 0xFF) ^ 0xFF) + 1;

	return(cksum);
}

/**
 * @brief Verify the checksum of a packet
 *
 * @param packet Pointer to a buffer where packet to verify is stored
 * @return boolean True is returned if checksum is valid
 */
static int checksum_verify(uint8_t *packet)
{
	uint32_t cksum;
	size_t len;
	int result = 0;

	/* Get data length into the packet header */
	len = packet[1];
	/* Compute checksum of the packet */
	cksum = checksum(packet, len + 2);

	/* Compare computed checksum with value into the packet itself */
	if (cksum == packet[len+2])
		result = 1;
	else
		printf("AQUAREA: Invalid RX checksum %.2X != %.2X\n", packet[len+2], cksum);

	return(result);
}
/* EOF */
