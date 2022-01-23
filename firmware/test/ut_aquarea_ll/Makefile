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
TARGET = unit_test
CC = gcc
CFLAGS = -Wall -g
CFLAGS += -Iinclude -I../../main

BUILDDIR = build
SRC = main.c log.c driver_uart.c
SRC += test_init.c test_rx.c test_cksum.c

COBJ = $(patsubst %.c, $(BUILDDIR)/%.o,$(SRC))

all: $(BUILDDIR) $(COBJ) aquarea_ll.o
	@echo "  [LD] $(TARGET)"
	@$(CC) -o $(TARGET) $(COBJ) aquarea_ll.o

clean:
	rm -f $(TARGET)
	rm -f $(BUILDDIR)/*.o aquarea_ll.o
	rm -f *~

$(BUILDDIR):
	@echo "  [MKDIR] $@"
	@mkdir $(BUILDDIR)

$(COBJ) : $(BUILDDIR)/%.o: %.c
	@echo "  [CC] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

aquarea_ll.o:
	@echo "  [CC] $@"
	@$(CC) $(CFLAGS) -c ../../main/aquarea_ll.c -o aquarea_ll.o