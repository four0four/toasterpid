PROGRAM=main
SOURCES=main.c lcd4bit.c
CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size
AVRDUDE=avrdude
MMCU=atmega168a
DEVICE=atmega168
F_CPU=8000000

PROGFLAGS= -p$(DEVICE) -c avrisp2 -P usb -U flash:w:$(PROGRAM).hex:i

CFLAGS=-mmcu=$(MMCU) -Wall -Os -g -std=c99 -DF_CPU=$(F_CPU)

all: $(PROGRAM).hex $(PROGRAM).lst

$(PROGRAM).hex: $(PROGRAM).out
	$(OBJCOPY) -j .text -j .data -O ihex $(PROGRAM).out $(PROGRAM).hex
	$(SIZE) -C $(PROGRAM).out --mcu=$(DEVICE)

$(PROGRAM).lst: $(PROGRAM).hex
	avr-objdump -h -S $(PROGRAM).out > $(PROGRAM).lst

$(PROGRAM).out: $(SOURCES)
	$(CC) $(CFLAGS) -I./ -o $(PROGRAM).out $(SOURCES)

flash: $(PROGRAM).hex
	$(AVRDUDE) $(PROGFLAGS)

clean:
	rm -f $(PROGRAM).lst
	rm -f $(PROGRAM).out
	rm -f $(PROGRAM).hex
