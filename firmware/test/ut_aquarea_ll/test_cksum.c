/**
 * @file  test_cksum.c
 * @brief Some tests to verify packet checksums
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
#include <string.h>
#include "aquarea_ll.h"
#include "driver_uart.h"
#include "log.h"

/* Functions for each sub-test */
static int test_nominal(void);
static int test_malformed(void);

/* Local variables for this group of tests */
extern int rx_switch;
static unsigned char rx_buffer[1024];
static int rx_result;

/**
 * @brief Entry point for this group of tests
 *
 */
int test_cksum(void)
{
	int result = 0;

	rx_switch = 2;

	/* Test with a well formed packet */
	if (test_nominal())
		result = -1;
	/* Test with a packet that contains a wrong checksum */
	if (test_malformed())
		result = -1;

	printf("\n");

	rx_switch = 0;

	return(result);
}

/**
 * @brief Aquarea RX handler during test_cksum
 *
 * When a packet is fully received, aquarea must call a function named
 * "aquarea_rx". During this test, each call to this special function are
 * forwarded to test_rx_rx. The goal of this function is to verify integrity
 * of the received packet (compare to data sent).
 */
void test_cksum_rx(unsigned char *packet, size_t len)
{
	unsigned char *pref;
	size_t len_sent;
	int i;

	/* If an error has already been reported */
	if (rx_result < 0)
		return;

	pref = rx_buffer;
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
		rx_result = 1;
}

unsigned char t2[203] = {
0x71,0xC8,0x01,0x10,0x56,0x55,0x52,0x49, 0x00,0x55,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x19,0x15,0x12,0x55, 0x15,0x9D,0x55,0x05,0x05,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80, 0x80,0x80,0xB4,0x71,0x71,0x97,0x99,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x80,0x85,0x15,0x8A,0x85,0x85,
0xD0,0x7B,0x78,0x1F,0x7E,0x1F,0x1F,0x79, 0x79,0x8D,0x8D,0xBC,0xAD,0x7B,0x8F,0xB7,
0xA3,0x7B,0x8F,0x98,0x85,0x76,0x8F,0x8A, 0x94,0x9E,0x8F,0x8A,0x94,0x9E,0x85,0x8F,
0x8A,0x11,0x3D,0x78,0xC1,0x0B,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x55,0xF5,
0x55,0x21,0xA9,0x15,0x59,0x05,0x12,0x12, 0x65,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0xE2,0xCF,0x0D,0x86,0x05,0x12,0xD0, 0x0C,0x95,0x05,0x97,0x00,0x00,0x9A,0x97,
0x97,0x32,0x32,0xAD,0xA3,0x32,0x32,0x32, 0x80,0xAD,0x97,0x92,0x98,0x97,0x9A,0x94,
0x94,0x95,0x97,0x46,0x01,0x01,0x01,0x00, 0x00,0x22,0x00,0x01,0x01,0x01,0x01,0x79,
0x01,0xC9,0xC9,0x01,0x1A,0x00,0x75,0x16, 0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x0D,
0x02,0x01,0x01,0x01,0x01,0x01,0x01,0x00, 0x00,0x00,0x9D};

static int test_nominal(void)
{
	int i;

	printf(COLOR_BLUE " * LL Checksum : test well formed packet " COLOR_NONE);

	log_start("/tmp/ut_log_cksum.txt");
	uart_init();
	rx_result = 0;

	if (aquarea_ll_init())
		goto error;

	for (i = 0; i < 100; i++)
	{
		if (i == 2)
		{
			memcpy(rx_buffer, t2, 203);
			uart_set_buffer(rx_buffer, 203);
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
	log_dump("/tmp/ut_log_cksum.txt");
	return(-1);
}

static int test_malformed(void)
{
	int i;

	printf(COLOR_BLUE " * LL Checksum : test packet with wrong checksum " COLOR_NONE);

	log_start("/tmp/ut_log_cksum.txt");
	uart_init();
	rx_result = 0;

	if (aquarea_ll_init())
		goto error;

	for (i = 0; i < 100; i++)
	{
		if (i == 2)
		{
			memcpy(rx_buffer, t2, 203);
			/* Insert data corruption */
			rx_buffer[10] = 0xFF;
			uart_set_buffer(rx_buffer, 203);
		}
		aquarea_ll_process();
	}
	if (rx_result != 0)
	{
		printf("Invalid packet has been accepted\n");
		goto error;
	}
	log_end();
	printf("[" COLOR_GREEN " OK " COLOR_NONE "]\n");
	return(0);

error:
	log_end();
	printf("[" COLOR_RED "FAIL" COLOR_NONE "]\n");
	log_dump("/tmp/ut_log_cksum.txt");
	return(-1);
}
/* EOF */
