/**
 * @file  main.c
 * @brief Entry point of this unit-test
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
#include "log.h"

/* Global variable used to route calls to "aquarea_rx" (see below) */
int rx_switch;

/* Declare functions for each group of tests */
int  test_init(void);
int  test_rx(void);
void test_rx_rx(unsigned char *packet, size_t len);
int  test_cksum(void);
void test_cksum_rx(unsigned char *packet, size_t len);

static void usage(char *appname);

/**
 * @brief Entry point of this unit-test
 *
 * @param argc Number or command line arguments
 * @param argv Array of string with command line arguments
 * @return integer Zero is returned on success, -1 for error
 */
int main(int argc, char **argv)
{
	int test_num;
	int result = 0;

	if (argc < 2)
	{
		usage(argv[0]);
		return(-1);
	}

	rx_switch = 0;
	log_init();

	test_num = atoi(argv[1]);

	if ((test_num == 1) || (test_num == 0))
	{
		if (test_init() != 0)
			result = -1;
	}
	if ((test_num == 2) || (test_num == 0))
	{
		if (test_rx() != 0)
			result = -1;
	}
	if ((test_num == 3) || (test_num == 0))
	{
		if (test_cksum() != 0)
			result = -1;
	}

	return(result);
}

/**
 * @brief Data reception handler
 *
 * The special function "aquarea_rx" is called by low-level layer when a
 * valid packet has been received. During tests, this function forward calls
 * to each specific test using the global rx_switch variable value.
 *
 * @param packet Pointer on byte array with received packet
 * @param len Number of bytes into the packet
 */
void aquarea_rx(unsigned char *packet, size_t len)
{
	if (rx_switch == 1)
		test_rx_rx(packet, len);
	else if (rx_switch == 2)
		test_cksum_rx(packet, len);
}

/**
 * @brief Print an help message about command line arguments
 *
 */
static void usage(char *appname)
{
	printf("Usage %s <test_num>\n", appname);
	printf("  where test_num can be:\n");
	printf("    0: Run all tests\n");
	printf("    1: Test uart_driver_install\n");
	printf("    2: Test data reception and processing\n");
	printf("    2: Test data checksums\n");
}
/* EOF */
