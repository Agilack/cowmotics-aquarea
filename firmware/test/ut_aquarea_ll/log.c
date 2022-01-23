/**
 * @file  log.c
 * @brief Redirect and save log messages during tests
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
#include <fcntl.h>
#include <unistd.h>
#include "log.h"

int old_1, new_1;

/**
 * @brief Initialize te log module
 *
 */
void log_init(void)
{
	old_1 = -1;
	new_1 = -1;
}

/**
 * @brief Start a log redirection session
 *
 * @param name Name of a temporary file where to save logs
 */
void log_start(char *name)
{
	// Sanity check
	if ((new_1 != -1) || (old_1 != -1))
		return;

	/* Flush now to avoid previous printf to be redirected */
	fflush(stdout);
	/* Open the temporary log file ... */
	new_1 = open(name, O_CREAT | O_RDWR | O_TRUNC, 0666);
	/* ... and redirect "stdout" into this file */
	old_1 = dup(1);
	dup2(new_1, 1);
}

/**
 * @brief Terminate a log session and close log file
 *
 */
void log_end(void)
{
	// Sanity check
	if (old_1 == -1)
		return;

	// Restore "stdout"
	dup2(old_1, 1);
	// Close temporary file descriptors
	close(old_1);
	old_1 = -1;
	close(new_1);
	new_1 = -1;
}

/**
 * @brief Dump to console the content of a log file
 *
 * @param name Name of the file to open/dump
 */
void log_dump(char *name)
{
	FILE *f;
	char  buffer[1024];

	fflush(stdout);

	f = fopen(name, "r");
	if (f == 0)
		return;

	while ( ! feof(f) )
	{
		memset(buffer, 0, 1024);
		fgets(buffer, 1024, f);
		write(1, buffer, strlen(buffer));
	}
	fclose(f);
}
/* EOF */
