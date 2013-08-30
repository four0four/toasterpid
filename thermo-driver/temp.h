#ifndef __TEMP_H
#define __TEMP_H

#include <avr/io.h>
#include <stdint.h>

// thermocouple amp AVR registers
#define AMP_CTL_PORT PORTD
#define AMP_CTL_PIN PIND
#define AMP_CTL_DDR DDRD

// thermocouple amp pin connections
#define AMP_CS 0
#define AMP_CLK 1
#define AMP_DATA 2

// thermocouple amp bitmasks
#define THERM_DATA 0xFFFC0000 
#define THERM_FAULT 0x00010000
#define THERM_INT_DATA 0x0000FFF0
#define THERM_SCV_FAULT 0x4
#define THERM_SCG_FAULT 0x2
#define THERM_OC_FAULT 0x1



#endif // __TEMP_H

