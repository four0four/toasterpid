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

/*
  clears cached temp data, then reads in latest from the amp
  all following functions require the locally stored data 
  - do not call this faster than the amplifier can handle
*/
void updateTemp();

/* 
  checks cached data for fault markers, then returns as follows
  3: SCV - thermocouple shorted to vcc
  2: SCG - thermocouple shorted to gnd
  1: OC  - thermocouple is open circuit
  0: No fault
*/
uint8_t getFaults();

/* 
  returns external thermocouple temperature 
  masked directly from cached data
*/
int16_t getExternalTemp();

/* 
  returns internal thermocouple temperature
  masked directly from cached data
*/
int16_t getInternalTemp();


#endif // __TEMP_H

