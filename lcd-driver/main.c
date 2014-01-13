// #include "lcd.h"
#include "lcd4bit.h"
#include <stdint.h>
#include <avr/io.h>

int main() {

/*
  DDRD = 0xFF;
  DDRB = 0xFF;
  PORTB = 0x00;
  PORTD = 0x00;
*/
  _delay_ms(150);

  lcdInit();

  lcdWriteString("Hello, world!\n");
  
  return 0;
}
