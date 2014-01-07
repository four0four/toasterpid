#include "lcd.h"
#include <stdint.h>
#include <avr/io.h>

int main() {


  DDRD = 0xFF;
  DDRB = 0xFF;
  PORTB = 0x00;
  PORTD = 0x00;

  _delay_ms(150);

  lcdInit();

  //_delay_ms(1);

  lcdWriteString("Hello, World!\n");

  return 0;
}
