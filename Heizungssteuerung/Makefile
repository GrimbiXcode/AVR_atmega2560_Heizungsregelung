PROJECT=$(shell basename $(shell pwd))

OBJS = $(PROJECT).o ../lcd_keypad_m2560/lcd_keypad_m2560.o

CC = avr-gcc
# Fedora 23 avr-libc is broken - use avr-gcc from Fedora 22 mounted on ~/x
CC = /usr/local/CrossPack-AVR/bin/avr-gcc

# for Arduino Mega2560
MCU = atmega2560	# must be lower case!!
ADUDE_IF = /dev/tty.usbmodem1411
ADUDE_BAUD = 115200
ADUDE_PROT = wiring

# for Arduino Nano and Arduino Uno (ev. ADUDE_BAUD = 115200 ?) 
#MCU = atmega328p
#ADUDE_IF = /dev/tty.usbmodem1411
#ADUDE_BAUD = 57600
#ADUDE_PROT = arduino

CFLAGS = -mmcu=$(MCU) -Wall -O2 -DF_CPU=16000000UL -gdwarf-2 

all: $(PROJECT).elf flash

# use different rule if assembler project source file '$(PROJECT).s' exists...
ifeq ($(PROJECT).s,$(wildcard $(PROJECT).s))
ASMDEFS = -D'__SFR_OFFSET=0' -D'low(x)=(x & 0xff)' -D'high(x)=(((x) >> 8) & 0xff)' -D'org=org 2*' -include 'avr/io.h' # macro org corrects .org statements because avr-as counts  counts program space in bytes whereas Atmel assembler in instructions
$(PROJECT).elf:$(PROJECT).s
	avr-gcc -mmcu=$(MCU) -nodefaultlibs -nostartfiles -I. -x assembler-with-cpp $(ASMDEFS) $<  -o $@
else
# normal linker rule for C project
$(PROJECT).elf: $(OBJS)
	$(CC) $(CFLAGS) $+ -o $@
endif

%.o: %.c
	$(CC) $(CFLAGS) -c $<  -o $@

%.hex: %.elf
	avr-objcopy -O ihex $<  $@

%.dis: %.elf
	avr-objdump  -D $<  > $@

flash: $(PROJECT).hex  $(ADUDE_IF)
	avrdude -F -c $(ADUDE_PROT) -p $(MCU) -P $(ADUDE_IF) -b $(ADUDE_BAUD) -U flash:w:$(PROJECT).hex:i


clean:
	rm -f $(PROJECT).dis $(PROJECT).elf $(PROJECT).hex 

.PHONY: clean flash

