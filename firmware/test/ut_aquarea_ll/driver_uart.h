/**
 * @file  driver_uart.h
 * @brief Headers and definition for special functions of UART simulation
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
#ifndef DRIVER_UART_H
#define DRIVER_UART_H

void uart_init(void);
void uart_set_buffer(unsigned char *src, int len);
int  uart_test_drv(int force);
int  uart_test_cfg(int force);
int  uart_test_pin(int force);

#endif
