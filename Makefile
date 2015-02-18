PROGRAM=toasterpid
SOURCES=toasterpid.c serial-driver/serial.c lcd-driver/lcd4bit.c thermo-driver/temp.c
INCLUDE=serial-driver/ -Ilcd-driver/ -Ithermo-driver/
CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size
AVRDUDE=avrdude
MMCU=atmega168a
DEVICE=atmega168
F_CPU=8000000
#F_CPU=12000000

PROGFLAGS= -p$(DEVICE) -c avrisp2 -P usb -U flash:w:$(PROGRAM).hex:i -B 1 -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0xf9:m 

CFLAGS=-mmcu=$(MMCU) -Wall -Os -g -std=c99 -DF_CPU=$(F_CPU) 

all: $(PROGRAM).hex $(PROGRAM).lst

$(PROGRAM).hex: $(PROGRAM).out
	$(OBJCOPY) -j .text -j .data -O ihex $(PROGRAM).out $(PROGRAM).hex
	$(SIZE) -C $(PROGRAM).out --mcu=$(DEVICE)

$(PROGRAM).lst: $(PROGRAM).hex
	avr-objdump -h -S $(PROGRAM).out > $(PROGRAM).lst

$(PROGRAM).out: $(SOURCES)
	$(CC) $(CFLAGS) -o $(PROGRAM).out $(SOURCES) -I$(INCLUDE)

flash: $(PROGRAM).hex
	#$(AVRDUDE) -p $(DEVICE) -c avrisp2 -P usb -B 1 parms
	$(AVRDUDE) -p $(DEVICE) -c avrisp2 -B 1 parms
	$(AVRDUDE) $(PROGFLAGS)

clean:
	rm -f $(PROGRAM).lst
	rm -f $(PROGRAM).out
	rm -f $(PROGRAM).hex
