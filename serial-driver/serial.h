#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>
#include <avr/io.h>

#define SERIAL_BAUD 9600
#define SERIAL_PRESCALER ((F_CPU/(16UL * SERIAL_BAUD)) - 1)

void serialInit();


void serialWrite(char data);

void serialString(char *data);

#endif // __SERIAL_H
