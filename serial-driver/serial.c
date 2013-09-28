#include "serial.h"

void serialInit() {
  // set prescaler
  UBRR0H = SERIAL_PRESCALER >> 8;
  UBRR0L = SERIAL_PRESCALER & 0xFF;
  // power on serial hw
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

void serialWrite(uint8_t data) {
  while(!UCSR0A & (1<<UDRE0));
  UDR0 = data;

}

void serialString(uint8_t *data) {
  serialWrite(*data);
  while(*(++data))
    serialWrite(*data);
}
