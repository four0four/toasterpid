#include "lcd.h"


void strobeEN() {
  LCD_CTL_PORT |= (1<<LCD_EN);
  __asm__ __volatile__("rjmp 1f\n 1:");
  LCD_CTL_PORT &= ~(1<<LCD_EN);
}

void lcdWait() {
  while(lcdRead(0) & 0x80);
}

void lcdWrite(uint8_t RS, uint8_t data) {
  lcdWriteNibble(RS, data >> 4);
  lcdWriteNibble(RS, data & 0x0F);
}

void lcdWriteNibble(uint8_t RS, uint8_t data) {
  // make sure direction is set
  LCD_CTL_DDR |= ((1<<LCD_EN)|(1<<LCD_RS)|(1<<LCD_RW));
  LCD_DATA_DDR |= ((1<<LCD_DATA_1)|(1<<LCD_DATA_2)|(1<<LCD_DATA_3)|(1<<LCD_DATA_4));
  // clear RW (write)
  LCD_CTL_PORT &= ~(1<<LCD_RW);
  // set EN low for now
  LCD_CTL_PORT &= ~(1<<LCD_EN);
  // conditional prevents non-1/0 screwups
  if(RS)
    LCD_CTL_PORT |= (1<<LCD_RS);
  else
    LCD_CTL_PORT &= ~(1<<LCD_RS);
  // write out data
  if(data & 0x01)
    LCD_DATA_PORT |= (1<<LCD_DATA_1);
  if(data & 0x02)
    LCD_DATA_PORT |= (1<<LCD_DATA_2);
  if(data & 0x04)
    LCD_DATA_PORT |= (1<<LCD_DATA_3);
  if(data & 0x08)
    LCD_DATA_PORT |= (1<<LCD_DATA_4);

  strobeEN();
}

uint8_t lcdRead(uint8_t RS) {
  // construct read byte - MSB first
  uint8_t res = lcdReadNibble(RS);
  res <<= 4;
  res |= lcdReadNibble(RS);

  return res;
}

uint8_t lcdReadNibble(uint8_t RS) {
  uint8_t data = 0;
  LCD_CTL_DDR |= ((1<<LCD_EN)|(1<<LCD_RS)|(1<<LCD_RW));
  LCD_DATA_DDR &= ~((1<<LCD_DATA_1)|(1<<LCD_DATA_2)|(1<<LCD_DATA_3)|(1<<LCD_DATA_4));
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
  if(LCD_DATA_PIN & 0x01)
    data |= (1<<LCD_DATA_1);
  if(LCD_DATA_PIN & 0x02)
    data |= (1<<LCD_DATA_2);
  if(LCD_DATA_PIN & 0x04)
    data |= (1<<LCD_DATA_3);
  if(LCD_DATA_PIN & 0x08)
    data |= (1<<LCD_DATA_4);
  
  return data;
}
