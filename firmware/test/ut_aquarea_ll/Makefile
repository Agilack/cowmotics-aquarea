##
 # @file  Makefile
 # @brief Script to compile this unit-test using "make" command
 #
 # @author Saint-Genest Gwenael <gwen@agilack.fr>
 # @copyright Agilack (c) 2022
 #
 # @page License
 # This firmware is free software: you can redistribute it and/or modify it
 # under the terms of the GNU General Public License version 3 as published
 # by the Free Software Foundation. You should have received a copy of the
 # GNU General Public License along with this program, see LICENSE.md file
 # for more details.
 # This program is distributed WITHOUT ANY WARRANTY.
##

CFLAGS = -Wall -g
CFLAGS += -Iinclude -I../../main

all:
	cc $(CFLAGS) -c main.c -o main.o
	cc $(CFLAGS) -c driver_uart.c -o driver_uart.o
	cc $(CFLAGS) -c ../../main/aquarea_ll.c -o aquarea_ll.o
	cc -o unit_test main.o driver_uart.o aquarea_ll.o

clean:
	rm -f unit_test
	rm -f *.o
	rm -f *~
