# -----------------------------------------------------------------------
# Copyright (c) 2010     OFFIS Institute for Information Technology,
#                        26121 Oldenburg, Germany
# 
# All rights reserved.
# 
# This file is directly or indirectly part of the OFFIS Virtual Platform.
# A simualtion of an ARM7tdmi micro processor with OVP (a), FreeRTOS
# operating system (b) and user defined tasks.
# 
# (a) http://www.ovpworld.org/
# (b) http://www.freertos.org/
# 
# Created for the European projects: Nephron+ (1)
# 
# 1) http://www.nephronplus.eu/
# 
# A list of authors and contributors can be found in the accompanying
# AUTHORS file.  For detailed copyright information, please refer
# to the COPYING file.
# 
# -----------------------------------------------------------------------
# Contact information:
#  OFFIS
#    Institute for Information Technology
#  Dipl.-Inform. Frank Poppen
#    Escherweg 2
#    D-26121 Oldenburg
#    Germany
#  www  : http://www.offis.de/
#  phone: +49 (441) 9722-230
#  fax  : +49 (441) 9722-128
# -----------------------------------------------------------------------
# 

OPTIM=-O3
RUN_MODE=RUN_FROM_ROM
LDSCRIPT=lpc2106-rom.ld

CC=arm-elf-gcc
OBJCOPY=arm-elf-objcopy
CRT0=boot.s

WARNINGS=-Wall -Wextra -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wsign-compare \
		-Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wunused

CFLAGS=$(WARNINGS) -D$(RUN_MODE) -DGCC_ARM7 -I. -I./Source/include \
		-I./Common/include  -I./Common/rtw_include -ggdb -mcpu=arm7tdmi -T$(LDSCRIPT) \
		 $(OPTIM) -fomit-frame-pointer -fno-strict-aliasing



LINKER_FLAGS=-Xlinker -lm -oOffisSimLinkDemo.elf -Xlinker -M -Xlinker -Map=OffisSimLinkDemo.map

#
# Source files that can be built to THUMB mode.
#
THUMB_SRC = \
main.c 

NEPHRON_SRC = \

#
# Source files that must be built to ARM mode.
#
ARM_SRC = \
portISR.c

#
# Define all object files.
#
ARM_OBJ = $(ARM_SRC:.c=.o)
THUMB_OBJ = $(THUMB_SRC:.c=.o)
NEPHRON_OBJ = $(NEPHRON_SRC:.c=.o)

OffisSimLinkDemo.hex : OffisSimLinkDemo.elf
	$(OBJCOPY) OffisSimLinkDemo.elf -O ihex OffisSimLinkDemo.hex

OffisSimLinkDemo.elf : $(ARM_OBJ) $(THUMB_OBJ) $(NEPHRON_OBJ) $(CRT0) Makefile
	$(CC) $(CFLAGS) $(ARM_OBJ) $(THUMB_OBJ) $(NEPHRON_OBJ) -nostartfiles $(CRT0) $(LINKER_FLAGS)

$(NEPHRON_OBJ) : %.o : %.c %.h nephron.h $(LDSCRIPT) Makefile
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(THUMB_OBJ) : %.o : %.c $(LDSCRIPT) Makefile
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@

$(ARM_OBJ) : %.o : %.c $(LDSCRIPT) Makefile
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	rm -rf $(ARM_OBJ) $(THUMB_OBJ) $(NEPHRON_OBJ) *.elf
