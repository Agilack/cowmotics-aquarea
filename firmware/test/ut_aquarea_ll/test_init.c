/**
 * @file  test_init.c
 * @brief Some tests to verify Aquarea UART initielisation
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
#include "aquarea_ll.h"
#include "driver_uart.h"
#include "log.h"

/* Functions for each sub-test */
static int test_nominal(void);
static int test_err_driver(void);
static int test_err_config(void);
static int test_err_pin(void);

/**
 * @brief Entry point for this group of tests
 *
 */
int test_init(void)
{
	int result = 0;

	/* Test nominal initialisation case */
	if (test_nominal())
		result = -1;
	/* Test with an error injected into UART driver */
	if (test_err_driver())
		result = -1;
	/* Test with an error injected into UART config */
	if (test_err_config())
		result = -1;
	/* Test with an error injected into GPIO (pin) config */
	if (test_err_pin())
		result = -1;

	printf("\n");

	return(result);
}

static int test_nominal(void)
{
	printf(COLOR_BLUE " * LL Initialization : test nominal case " COLOR_NONE);

	log_start("/tmp/ut_log_init.txt");
	uart_init();

	if (aquarea_ll_init() != 0)
		goto error;

	// Driver must have been started
	if (uart_test_drv(0) != 1)
		goto error;
	// Pins must have been defined
	if (uart_test_pin(0) != 1)
		goto error;
	// A valid config must have been set
	if (uart_test_cfg(0) != 1)
		goto error;

	log_end();
	printf("[" COLOR_GREEN " OK " COLOR_NONE "]\n");
	return(0);
error:
	log_end();
	printf("[" COLOR_RED "FAIL" COLOR_NONE "]\n");
	log_dump("/tmp/ut_log_init.txt");
	return(-1);
}

static int test_err_config(void)
{
	printf(COLOR_BLUE " * LL Initialization : test config error case " COLOR_NONE);

	log_start("/tmp/ut_log_init.txt");
	uart_init();
	uart_test_cfg(1);

	if (aquarea_ll_init() == 0)
		goto error;

	// driver install should have been called
	if (uart_test_drv(0) != 1)
		goto error;
	// Pin config can be called or omit but must not be in error
	if (uart_test_pin(0) == -1)
		goto error;
	// Uart config can be called or omit but must not be in error
	if (uart_test_cfg(0) == -1)
		goto error;

	log_end();
	printf("[" COLOR_GREEN " OK " COLOR_NONE "]\n");
	return(0);
error:
	log_end();
	printf("[" COLOR_RED "FAIL" COLOR_NONE "]\n");
	log_dump("/tmp/ut_log_init.txt");
	return(-1);
}

static int test_err_driver(void)
{
	printf(COLOR_BLUE " * LL Initialization : test driver error case " COLOR_NONE);

	log_start("/tmp/ut_log_init.txt");
	uart_init();
	uart_test_drv(1);

	if (aquarea_ll_init() == 0)
		goto error;

	// driver install should have been called
	if (uart_test_drv(0) != 1)
		goto error;
	// Pin config can be called or omit but must not be in error
	if (uart_test_pin(0) == -1)
		goto error;
	// Uart config can be called or omit but must not be in error
	if (uart_test_cfg(0) == -1)
		goto error;

	log_end();
	printf("[" COLOR_GREEN " OK " COLOR_NONE "]\n");
	return(0);
error:
	log_end();
	printf("[" COLOR_RED "FAIL" COLOR_NONE "]\n");
	log_dump("/tmp/ut_log_init.txt");
	return(-1);
}

static int test_err_pin(void)
{
	printf(COLOR_BLUE " * LL Initialization : test pin error case " COLOR_NONE);

	log_start("/tmp/ut_log_init.txt");
	uart_init();
	uart_test_pin(1);

	if (aquarea_ll_init() == 0)
		goto error;

	// driver install should have been called
	if (uart_test_drv(0) != 1)
		goto error;
	// Pin config can be called or omit but must not be in error
	if (uart_test_pin(0) == -1)
		goto error;
	// Uart config can be called or omit but must not be in error
	if (uart_test_cfg(0) == -1)
		goto error;

	log_end();
	printf("[" COLOR_GREEN " OK " COLOR_NONE "]\n");
	return(0);
error:
	log_end();
	printf("[" COLOR_RED "FAIL" COLOR_NONE "]\n");
	log_dump("/tmp/ut_log_init.txt");
	return(-1);
}
/* EOF */
