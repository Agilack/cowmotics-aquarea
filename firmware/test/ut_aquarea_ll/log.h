/**
 * @file  log.h
 * @brief Headers and definitions for the log module
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
#ifndef LOG_H
#define LOG_H

#define COLOR_NONE   "\x1B[0m"
#define COLOR_RED    "\x1B[31m"
#define COLOR_GREEN  "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_BLUE   "\x1B[34m"

void log_init(void);
void log_start(char *name);
void log_end(void);
void log_dump(char *name);

#endif
