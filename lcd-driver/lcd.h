#ifndef __LCD_H
#define __LCD_H

#include <avr/io.h>
#include <stdint.h>


#define LCD_CTL_PORT PORTD
#define LCD_E
#define LCD_RS

// the following assumes the use of the lower nibble
#define LCD_DATA_PORT PORTB


void writeCMD(uint8_t);

void writeData(uint8_t);

#endif // __LCD_H
