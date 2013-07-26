#ifndef __LCD_H
#define __LCD_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// What lcd.h provides: 
// writeData() and writeCMD() - basic communication functonality
// writeString() and writeChar() - abstracted write functionality
// Macros for relevant opcodes
// Configuration to fit hardware

// What lcd.h does not provide: initialization functions

// All delays are provided using simple timing loops from delay.h - nothing fancy

// control lines
#define LCD_CTL_PORT PORTD
#define LCD_E
#define LCD_RS

// data lines/bus
#define LCD_DATA_PORT PORTB
#define LCD_DATA_MASK 0x0F

// basic LCD commands. for commands with following options, simply bitwise OR the command w/the base
#define LCD_CLR 0x01 // clear the LCD entirely, cursor pos = 0
#define LCD_HOME 0x02 // return cursor position, reset shifting position
// - LCD entry mode control
#define LCD_ENTRY 0x04 // entry mode base
#define ENTRY_INCREMENT 0x02 // set to increment cursor during write
#define ENTRY_SHIFT 0x01 // set to shift display
// - LCD power control
#define LCD_DISPLAY 0x08 // power control base
#define DISPLAY_ON 0x04 // set to power on, clear to power off
#define DISPLAY_CURSOR 0x02 // set to solid cursor, clear for no cursor (check)
#define DISPLAY_BLINK_CURSOR 0x01 // set to blink LCD cursor (check as well)
// - LCD cursor control
#define LCD_CURSOR 0x10 // control direction/behavior of cursor
#define CURSOR_SHIFT 0x08 // set to shift display (default/unset is move)
#define CURSOR_SHIFT_RIGHT 0x04 // set to shift to the right while writing (default/unset is left)
// - LCD function control
#define LCD_FUNCTION 0x20 // misc configuration
#define FUNCTION_DATA_WIDTH 0x10 // set to indicate 8 bit interface, unset/zero for 4 bit
#define FUNCTION_DISPLAY_LINES 0x08 // set to indicate 2 lines, unset for 1
#define FUNCTION_FONT 0x04 // set to indicate 5*10 font, unset for 5*8


void writeCMD(uint8_t);

void writeData(uint8_t);

#endif // __LCD_H
