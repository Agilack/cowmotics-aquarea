/**
 * @file  test_rx.c
 * @brief Some tests to verify how RX packets are handled
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
#include <stdlib.h>
#include "aquarea_ll.h"
#include "driver_uart.h"
#include "log.h"

/* Functions for each sub-test */
static int test_single(void);
static int test_fragmented(void);
static int test_multiple(void);
static int test_large(void);

/* Local variables for this group of tests */
extern int rx_switch;
static unsigned char rx_buffer[1024];
static int rx_offset;
static int rx_result;

/**
 * @brief Entry point for this group of tests
 *
 */
int test_rx(void)
{
	int result = 0;

	rx_switch = 1;

	/* Test with a single non-fragmented packet */
	if (test_single())
		result = -1;
	/* Test with a single fragmented packet */
	if (test_fragmented())
		result = -1;
	/* Test with multiple packets */
	if (test_multiple())
		result = -1;
	/* Test with largest possible packet */
	if (test_large())
		result = -1;

	printf("\n");

	rx_switch = 0;

	return(result);
}

/**
 * @brief Aquarea RX handler during test_rx
 *
 * When a packet is fully received, aquarea must call a function named
 * "aquarea_rx". During this test, each call to this special function are
 * forwarded to test_rx_rx. The goal of this function is to verify integrity
 * of the received packet (compare to data sent).
 */
void test_rx_rx(unsigned char *packet, size_t len)
{
	unsigned char *pref;
	size_t len_sent;
	int i;

	/* If an error has already been reported */
	if (rx_result < 0)
		return;

	pref = (rx_buffer + rx_offset);
	len_sent = (pref[1] + 3);

	/* First, verify that received length is equal to packet sent */
	if (len != len_sent)
	{
		printf(COLOR_RED "RX: Received len %d differ (%d sent)" COLOR_NONE "\n", (int)len, (int)len_sent);
		rx_result = -1;
		return;
	}
	/* Then, compare received packet with data sent */
	for (i = 0; i < len; i++)
	{
		if (packet[i] != pref[i])
		{
			printf(COLOR_RED "RX: Corrupted content at position %d (%.2X != %.2X)" COLOR_NONE "\n", i, packet[i], pref[i]);
			rx_result = -2;
			break;
		}
	}
	if (i == len)
	{
		rx_result = 1;
		rx_offset += len_sent;
	}
}

static int test_single(void)
{
	int i, j;

	printf(COLOR_BLUE " * LL Reception : single non-framented packet " COLOR_NONE);

	log_start("/tmp/ut_log_rx.txt");
	uart_init();
	rx_result = 0;
	rx_offset = 0;

	if (aquarea_ll_init())
		goto error;

	for (i = 0; i < 100; i++)
	{
		if (i == 2)
		{
			for (j = 0; j < 128; j++)
				rx_buffer[j] = j;
			rx_buffer[1]  = 20;
			rx_buffer[22] = 0x06;
			uart_set_buffer(rx_buffer, 23);
		}
		aquarea_ll_process();
	}
	if (rx_result != 1)
	{
		printf("Packet not received or bad packet\n");
		goto error;
	}
	log_end();
	printf("[" COLOR_GREEN " OK " COLOR_NONE "]\n");
	return(0);

error:
	log_end();
	printf("[" COLOR_RED "FAIL" COLOR_NONE "]\n");
	log_dump("/tmp/ut_log_rx.txt");
	return(-1);
}

static int test_fragmented(void)
{
	int i, j;

	printf(COLOR_BLUE " * LL Reception : single *framented* packet " COLOR_NONE);

	log_start("/tmp/ut_log_rx.txt");
	uart_init();
	rx_result = 0;
	rx_offset = 0;

	if (aquarea_ll_init())
		goto error;

	for (i = 0; i < 100; i++)
	{
		if (i == 2)
		{
			for (j = 0; j < 128; j++)
				rx_buffer[j] = j;
			rx_buffer[1] = 20;
			rx_buffer[22] = 0x06;
			uart_set_buffer(rx_buffer, 10);
		}
		if (i == 20)
			uart_set_buffer(rx_buffer+10, 13);
		aquarea_ll_process();
	}
	if (rx_result != 1)
	{
		printf("Packet not received or bad packet\n");
		goto error;
	}
	log_end();
	printf("[" COLOR_GREEN " OK " COLOR_NONE "]\n");
	return(0);

error:
	log_end();
	printf("[" COLOR_RED "FAIL" COLOR_NONE "]\n");
	log_dump("/tmp/ut_log_rx.txt");
	return(-1);
}

static int test_large(void)
{
	int i, j;

	printf(COLOR_BLUE " * LL Reception : test largest possible packet " COLOR_NONE);

	log_start("/tmp/ut_log_rx.txt");
	uart_init();
	rx_result = 0;
	rx_offset = 0;

	if (aquarea_ll_init())
		goto error;

	for (i = 0; i < 100; i++)
	{
		if (i == 2)
		{
			for (j = 0; j < 1024; j++)
				rx_buffer[j] = j;
			/* Size of packet */
			rx_buffer[1] = 255;
			rx_buffer[257] = 0x82;
			uart_set_buffer(rx_buffer, 258);
		}
		aquarea_ll_process();
	}
	if (rx_result != 1)
	{
		printf("Packet not received or bad packet\n");
		goto error;
	}
	log_end();
	printf("[" COLOR_GREEN " OK " COLOR_NONE "]\n");
	return(0);

error:
	log_end();
	printf("[" COLOR_RED "FAIL" COLOR_NONE "]\n");
	log_dump("/tmp/ut_log_rx.txt");
	return(-1);
}

static int test_multiple(void)
{
	int i, j;

	printf(COLOR_BLUE " * LL Reception : two packets received simultanously " COLOR_NONE);

	log_start("/tmp/ut_log_rx.txt");
	uart_init();
	rx_result = 0;
	rx_offset = 0;

	if (aquarea_ll_init())
		goto error;

	for (i = 0; i < 100; i++)
	{
		if (i == 2)
		{
			for (j = 0; j < 128; j++)
				rx_buffer[j] = j;
			/* Size of first packet */
			rx_buffer[1] = 20;
			rx_buffer[22] = 0x06;
			/* Size of second packet */
			rx_buffer[24] = 10;
			rx_buffer[35] = 0xB8;
			uart_set_buffer(rx_buffer, 36);
		}
		aquarea_ll_process();
	}
	if (rx_result != 1)
	{
		printf("Packet not received or bad packet\n");
		goto error;
	}
	log_end();
	printf("[" COLOR_GREEN " OK " COLOR_NONE "]\n");
	return(0);

error:
	log_end();
	printf("[" COLOR_RED "FAIL" COLOR_NONE "]\n");
	log_dump("/tmp/ut_log_rx.txt");
	return(-1);
}
/* EOF */
