#define F_CPU 1000000

#include "lcd.h"
#include <stdint.h>
#include <avr/io.h>


void waitLCD() {
  DDRB = 0x00;
  PORTB |= (1<<LCD_RW);
  PORTB &= ~(1<<LCD_RS);
  while(PINB & (1<<3));
  DDRB = 0xFF;
  PORTB &= ~(1<<LCD_RW);
}

int main() {


  DDRD = 0xFF;
  DDRB = 0xFF;
  PORTB = 0x00;
  PORTD = 0x00;


  _delay_ms(110);
  lcdWrite(0, 0x3);
  _delay_ms(5);
  lcdWrite(0, 0x3);
  _delay_ms(5);
  lcdWrite(0, 0x3);
  _delay_ms(5);
  lcdWrite(0, 0x2);
  _delay_ms(3);

  lcdWrite(0,0x28);
  lcdWait();
  lcdWrite(0,0xC);
  lcdWait();
  lcdWrite(0,1);
  lcdWait();
  lcdWrite(0,0x06);
  lcdWait();


  //_delay_ms(1);

  while(1){
    lcdWrite(1,'!');
    lcdWait();
  }

  return 0;
}
