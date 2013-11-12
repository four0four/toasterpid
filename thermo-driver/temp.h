#ifndef __TEMP_H
#define __TEMP_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// thermocouple amp AVR registers
#define AMP_CTL_PORT PORTB
#define AMP_CTL_PIN PINB
#define AMP_CTL_DDR DDRB

// thermocouple amp pin connections
#define AMP_CS 0
#define AMP_CLK 5
#define AMP_DATA 4

// thermocouple amp bitmasks
#define THERM_DATA 0x7FFC0000UL
#define THERM_FAULT 0x00010000UL
#define THERM_INT_DATA 0x0000FFF0UL
#define THERM_FAULT_MASK 0x7
#define THERM_SCV_FAULT 0x4
#define THERM_SCG_FAULT 0x2
#define THERM_OC_FAULT 0x1

/*
  Initializes ports required
*/
void initThermo();

/*
  returns value directly read off the SPI interface - bit banged in software
*/
uint32_t readTemp();

/* 
  checks cached data for fault markers, then returns the error code(s)
*/
uint8_t getFaults(uint32_t);

/* 
  returns external thermocouple temperature 
  masked directly from cached data
*/
int16_t getExternalTemp(uint32_t);

/* 
  returns internal thermocouple temperature
  masked directly from cached data
*/
int16_t getInternalTemp(uint32_t);

#endif // __TEMP_H

