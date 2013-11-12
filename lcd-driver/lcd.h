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

// All delays are provided using simple timing loops from delay.h - nothing fancy

// Everything assumes 8 bit now

// control lines
#define LCD_CTL_PORT PORTC
#define LCD_CTL_DDR DDRC
#define LCD_EN 2
#define LCD_RS 1
#define LCD_RW 0

// data lines/bus
#define LCD_DATA_PORT PORTB
#define LCD_DATA_DDR DDRB
#define LCD_DATA_PIN PINB

#define LCD_DATA_MASK 0xFF;

// RS states
#define RS_CMD 0
#define RS_DATA 1

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

void strobeEN();


void lcdInit();

void lcdWrite(uint8_t, uint8_t);

uint8_t lcdRead(uint8_t);

void lcdWait(); 


#endif // __LCD_H
