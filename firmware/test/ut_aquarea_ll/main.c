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

/* Declare private uart driver functions */
void uart_init(void);
void uart_set_buffer(unsigned char *src, int len);

static void test_rx(void);
static void usage(char *appname);

int main(int argc, char **argv)
{
	int test_num;

	if (argc < 2)
	{
		usage(argv[0]);
		return(-1);
	}

	test_num = atoi(argv[1]);
	switch(test_num)
	{
		case 1:
			printf("Test 1 is valid but nom implemented yet\n");
			break;
		case 2:
			test_rx();
			break;
		default:
			printf("Invalid test specified \"%s\"\n\n", argv[1]);
			usage(argv[0]);
	}
}

unsigned char t1[1024];

static void test_rx(void)
{
	int i, j;

	aquarea_ll_init();

	printf("TEST: Simulate one full valid packet\n");
	for (i = 0; i < 100; i++)
	{
		if (i == 2)
		{
			for (j = 0; j < 128; j++)
				t1[j] = j;
			t1[1] = 20;
			uart_set_buffer(t1, 23);
		}
		aquarea_ll_process();
	}
	printf("\n");

	printf("TEST: Simulate one full packet, sent with pause\n");
	for (i = 0; i < 100; i++)
	{
		if (i == 2)
		{
			for (j = 0; j < 128; j++)
				t1[j] = j;
			t1[1] = 20;
			uart_set_buffer(t1, 10);
		}
		if (i == 20)
			uart_set_buffer(t1+10, 13);
		aquarea_ll_process();
	}
	printf("\n");

	printf("TEST: Simulate two full packets into RX buffer\n");
	for (i = 0; i < 100; i++)
	{
		if (i == 2)
		{
			for (j = 0; j < 128; j++)
				t1[j] = j;
			/* Size of first packet */
			t1[1] = 20;
			/* Size of second packet */
			t1[24] = 10;
			uart_set_buffer(t1, 36);
		}
		aquarea_ll_process();
	}
	printf("\n");

	printf("TEST: Simulate one packet too big for buffer\n");
	for (i = 0; i < 100; i++)
	{
		if (i == 2)
		{
			for (j = 0; j < 1024; j++)
				t1[j] = j;
			/* Size of packet */
			t1[1] = 255;
			uart_set_buffer(t1, 258);
		}
		aquarea_ll_process();
	}
	printf("\n");
}

static void usage(char *appname)
{
	printf("Usage %s <test_num>\n", appname);
	printf("  where test_num can be:\n");
	printf("    1: Test uart_driver_install\n");
	printf("    2: Test data reception and processing\n");
}
/* EOF */
