#include "serial.h"

void serialInit() {
  // set prescaler
  UBRR0H = (SERIAL_PRESCALER >> 8);
  UBRR0L = SERIAL_PRESCALER;
  UCSR0C |= (3 << UCSZ00);
  // power on serial hw
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
}

void serialWrite(char data) {
  while(!(UCSR0A & (1<<UDRE0)));
  UDR0 = data;
}

void serialString(char *data) {
  while(*data != 0){
    serialWrite(*data);
    data++;
  }
}
