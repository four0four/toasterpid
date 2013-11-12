PROGRAM=main
SOURCES=main.c lcd.c
CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size
AVRDUDE=avrdude
MMCU=attiny4313
F_CPU=1000000

PROGFLAGS= -p$(DEVICE) -c avrisp2 -p usb -U flash:w:$(PROGRAM).hex:i

CFLAGS=-mmcu=$(MMCU) -Wall -Os -g -std=c99 -DF_CPU=$(F_CPU)

all: $(PROGRAM).hex $(PROGRAM).lst

$(PROGRAM).hex: $(PROGRAM).out
	$(OBJCOPY) -j .text -j .data -O ihex $(PROGRAM).out $(PROGRAM).hex
	$(SIZE) -C $(PROGRAM).elf --mcu=$(DEVICE)

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