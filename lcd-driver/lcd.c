#include "lcd.h"


void strobeEN() {
  LCD_CTL_PORT &= ~(1<<LCD_EN);
  _delay_us(10);
  LCD_CTL_PORT |= (1<<LCD_EN);
  _delay_us(10);
  LCD_CTL_PORT &= ~(1<<LCD_EN);
}

void lcdWait() {
  while(lcdRead(0) & 0x80);
}

void lcdInit() {
  // initialize data pins for 8 bit use
  LCD_CTL_DDR |= (1<<LCD_EN) | (1<<LCD_RS) | (1<<LCD_RW);
  LCD_DATA_DDR |= LCD_DATA_MASK;
  
  // initial pin conditions
  LCD_CTL_PORT |= (1<<LCD_EN);
  _delay_ms(20); // additional wait

  lcdWrite(0, 0x30);
  _delay_ms(6);
  lcdWrite(0, 0x30);
  _delay_ms(2);
  lcdWrite(0, 0x30);
  _delay_ms(2);
  // now in 8-bit mode
  lcdWrite(0, 0x38); // Function Set
  _delay_us(100);
  lcdWrite(0, 0x08); // Start w/lcd off
  _delay_us(100);
  lcdWrite(0, 0x01); // Clear ddram/display
  _delay_ms(5);
  lcdWrite(0, 0x06); // Entry Mode Set
  _delay_us(100);
  // basic init done!
 lcdWrite(0, 0x0C); // turn lcd on
  _delay_us(100);
//  lcdWrite(0, 0x80);
}

void lcdWrite(uint8_t RS, uint8_t data) {
  // make sure direction is set
  LCD_CTL_DDR |= ((1<<LCD_EN)|(1<<LCD_RS)|(1<<LCD_RW));
  LCD_DATA_DDR |= LCD_DATA_MASK;
  // clear RW (write)
  LCD_CTL_PORT &= ~(1<<LCD_RW);
  // conditional prevents non-1/0 screwups
  if(RS)
    LCD_CTL_PORT |= (1<<LCD_RS);
  else
    LCD_CTL_PORT &= ~(1<<LCD_RS);

  LCD_DATA_PORT = data;

  strobeEN();
  _delay_us(20);
}

uint8_t lcdRead(uint8_t RS) {
  uint8_t data = 0;
  LCD_CTL_DDR |= ((1<<LCD_EN)|(1<<LCD_RS)|(1<<LCD_RW));
  LCD_DATA_DDR &= ~LCD_DATA_MASK;
  LCD_CTL_PORT &= ~(1<<LCD_EN);
  // conditional prevents non-1/0 screwups
  if(RS)
    LCD_CTL_PORT |= (1<<LCD_RS);
  else
    LCD_CTL_PORT &= ~(1<<LCD_RS);
  // set RW (read)
  LCD_CTL_PORT |= (1<<LCD_RW);
  _delay_ms(3);
  LCD_CTL_PORT |= (1<<LCD_EN);
  _delay_ms(3);

  data = LCD_DATA_PIN;

  return data;
}

