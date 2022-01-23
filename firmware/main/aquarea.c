/**
 * @file  main/aquarea.c
 * @brief Main functions to handle Aquarea
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
#include <time.h>
#include "aquarea_ll.h"

static void aquarea_send_query(void);

static time_t tm_ref;

/**
 * @brief Initialize the Aquarea module
 *
 * This function initialize all things used to handle Aquarea. This function
 * must be called before any other function of this module.
 */
void aquarea_init(void)
{
	/* Call sublayer for low-level inits */
	aquarea_ll_init();

	/* Get the current time (will be used to compute delay) */
	time(&tm_ref);
}

/**
 * @brief Do priodic tasks to cntrol and handle Aquarea
 *
 * This function must be called periodically to process Aquarea events.
 */
void aquarea_process(void)
{
	time_t  tm_now;

	aquarea_ll_process();

	/* If 5 sec elapsed since last query, query again */
	time(&tm_now);
	if ((tm_now - tm_ref) > 5)
	{
		aquarea_send_query();
		/* Save a new time ref */
		tm_ref = tm_now;
	}
}

void aquarea_rx(uint8_t *packet, size_t len)
{
	int i;
	printf("AQUAREA: Received packet :\n");
	for (i = 0; i < len; i++)
	{
		printf(" %.2X", packet[i]);
		if ((i & 0x0F) == 0x0F)
			printf("\n");
	}
	printf("\n");
}

/* -------------------------------------------------------------------------- */
/* --                          Internal functions                          -- */
/* -------------------------------------------------------------------------- */

/**
 * @brief Send a packet to ask Aquarea for his status
 *
 */
static void aquarea_send_query(void)
{
	uint8_t req[260];

	printf("Send a query packet ...\r\n");

	memset(req, 0, 260);
	/* Insert request header */
	req[0] = 0x71;
	req[1] = 108;  // Payload length
	req[2] = 0x01;
	req[3] = 0x10;

	aquarea_ll_send(req);
}
/* EOF */
