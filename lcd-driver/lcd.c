#include "lcd.h"

void strobeEN() {
  LCD_CTL_PORT &= ~(1<<LCD_EN);
  LCD_CTL_PORT |= (1<<LCD_EN);
  _delay_us(5);
  LCD_CTL_PORT &= ~(1<<LCD_EN);
  //_delay_us(35);
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

  lcdWrite(0, 0x30);
  _delay_ms(6);
  lcdWrite(0, 0x30);
  _delay_ms(2);
  lcdWrite(0, 0x30);
  _delay_ms(2);

  // now in 8-bit mode
  lcdWrite(RS_CMD, LCD_FUNCTION | FUNCTION_DATA_WIDTH | FUNCTION_DISPLAY_LINES);  // set up 2 lines, 8 bit bus, 5x8 font
  lcdWait();
  lcdWrite(RS_CMD, LCD_POWER); // LCD off (clean reset at least)
  lcdWait();
  lcdWrite(RS_CMD, LCD_CLR); // clear display/buffer
  lcdWait();
  lcdWrite(RS_CMD, LCD_ENTRY | ENTRY_INCREMENT); // set to increment on write
  lcdWait();
  // basic init done!
  lcdWrite(RS_CMD, LCD_POWER | DISPLAY_ON | DISPLAY_CURSOR); // turn on LCD w/o cursor
  lcdWait();
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
  lcdWait();
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
  LCD_CTL_PORT |= (1<<LCD_EN);
  _delay_us(10);

  data = LCD_DATA_PIN;

  return data;
}

void lcdWriteChar(char data) {
  lcdWrite(RS_DATA, data);
}

void lcdWriteString(char *data) {
  uint8_t sAddr = lcdRead(RS_CMD) & 0x7F;
  uint8_t pos;

  for(pos = sAddr; pos < 16; ++pos) {
    lcdWrite(RS_DATA, *data);
    ++data;
    if((!*data) | (*data == '\n')) 
      break;
  }
  if(*data) {
    // newline?
    if(*data == '\n')
      ++data;

    // work on line two
    lcdWrite(RS_CMD, 0xC0); // DDRAM address to line 2, pos 0
    lcdWait();
    for(pos = 0; pos < 16; ++pos) {
      lcdWrite(RS_DATA, *data);
      ++data;
      if((!*data) | (*data == '\n'))
        break;
    }
  }
}

void lcdWriteLine(uint8_t line, uint8_t pos, char *data) {
  if(line == 0)
    lcdWrite(RS_CMD, 0x80 + pos);
  else
    lcdWrite(RS_CMD, 0xC0 + pos);
  while((*data) && (pos < 16)) {
    lcdWrite(RS_DATA, *data);
    ++pos;
    ++data;
  }
}
