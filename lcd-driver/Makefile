SOURCES=main.c lcd.c
CC=avr-gcc
OBJCOPY=avr-objcopy
MMCU=attiny4313
F_CPU=1000000

CFLAGS=-mmcu=$(MMCU) -Wall -Os -g -std=c99 -DF_CPU=$(F_CPU)

all: main.hex main.lst

main.hex: main.out
	$(OBJCOPY) -j .text -O ihex main.out main.hex

main.lst: main.hex
	avr-objdump -h -S main.out > main.lst

main.out: $(SOURCES)
	$(CC) $(CFLAGS) -I./ -o main.out $(SOURCES)

clean:
	rm -f main.lst
	rm -f main.out
	rm -f main.hex
