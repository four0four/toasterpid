#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>

#define SERIAL_BAUD 9600
#define SERIAL_PRESCALER (SERIAL_BAUD/(16 * SERIAL_BAUD)) - 1

void serialInit();


void serialWrite(uint8_t);

void serialString(*uint8_t);

#endif // __SERIAL_H
