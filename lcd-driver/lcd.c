#include "lcd.h"


void writeCMD(uint8_t cmd) {
  // EN low, RS low for cmd
  LCD_CTL_PORT &= ~((1<<LCD_E)|(1<<LCD_RS));
  // clear data lines for high nibble
  LCD_DATA_PORT &= ~LCD_DATA_MASK; 
  LCD_DATA_PORT |= (cmd & 0xF0) & LCD_DATA_MASK;
  // allow read
  LCD_CTL_PORT |= (1<<LCD_E);
  // need to wait ~500ns, double is overkill anyway
  _delay_us(1);                                   
  LCD_CTL_PORT &= ~(1<<LCD_E);
  _delay_ms(6);
  // clear data lines for low nibble
  LCD_DATA_PORT &= ~LCD_DATA_MASK; 
  LCD_DATA_PORT |= (cmd & 0x0F) & LCD_DATA_MASK;
  _delay_us(1);                                   
  // clear EN to finish
  LCD_CTL_PORT &= ~(1<<LCD_E);
  _delay_ms(6);
}

void writeData(uint8_t data) {
  // EN low, RS high for data
  LCD_CTL_PORT &= ~(1<<LCD_E);
  LCD_CTL_PORT |= (1<<LCD_RS);
  // clear data lines for high nibble
  LCD_DATA_PORT &= ~LCD_DATA_MASK; 
  LCD_DATA_PORT |= (data & 0xF0) & LCD_DATA_MASK;
  // allow read
  LCD_CTL_PORT |= (1<<LCD_E);
  // need to wait ~500ns, double is overkill anyway
  _delay_us(1);                                   
  LCD_CTL_PORT &= ~(1<<LCD_E);
  _delay_us(500);
  // clear data lines for low nibble
  LCD_DATA_PORT &= ~LCD_DATA_MASK; 
  LCD_DATA_PORT |= (data & 0x0F) & LCD_DATA_MASK;
  _delay_us(1);                                   
  // clear EN to finish
  LCD_CTL_PORT &= ~(1<<LCD_E);
  _delay_us(500);
}
