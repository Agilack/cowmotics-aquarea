/**
 * @file  main/aquarea_ll.h
 * @brief Headers and definitions for Aquarea low level functions
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
#ifndef AQUAREA_LL_H
#define AQUAREA_LL_H
#include "driver/uart.h"

#define AQUAREA_UART UART_NUM_2

void aquarea_ll_init(void);
void aquarea_ll_process(void);

#endif
