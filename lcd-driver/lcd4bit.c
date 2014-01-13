#include "lcd4bit.h"

void strobeEN() {
  LCD_CTL_PORT &= ~(1<<LCD_EN);
  LCD_CTL_PORT |= (1<<LCD_EN);
  // One of the following *may* be required at faster clocks:
  //  _delay_us(1);
  //__asm__("nop");
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
  LCD_CTL_PORT &= ~(1<<LCD_RS);

  lcdWrite(0, 0x30);
  _delay_ms(6);
  lcdWrite(0, 0x30);
  _delay_ms(2);
  lcdWrite(0, 0x30);
  _delay_ms(2);

  // now in 4-bit mode
  lcdWrite(RS_CMD, LCD_FUNCTION | FUNCTION_DISPLAY_LINES);  // set up 2 lines, 8 bit bus, 5x8 font
  lcdWait();
  lcdWrite(RS_CMD, LCD_POWER); // LCD off (clean reset at least)
  lcdWait();
  lcdWrite(RS_CMD, LCD_CLR); // clear display/buffer
  lcdWait();
  lcdWrite(RS_CMD, LCD_ENTRY | ENTRY_INCREMENT); // set to increment on write
  lcdWait();
  // basic init done!
  lcdWrite(RS_CMD, LCD_POWER | DISPLAY_ON); // turn on LCD w/o cursor
  lcdWait();
}

void lcdWriteNibble(uint8_t data){
  LCD_DATA_PORT &= ~LCD_DATA_MASK;
  LCD_DATA_PORT |= data;
  strobeEN();
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

  // push first (high) nibble
  lcdWriteNibble(data >> 4);
  // push second (low) nibble
  lcdWriteNibble(data & 0x0F);

  lcdWait();
}

uint8_t lcdRead(uint8_t RS) {
  uint8_t data = 0;
  LCD_CTL_DDR |= ((1<<LCD_EN)|(1<<LCD_RS)|(1<<LCD_RW));
  LCD_DATA_DDR &= ~LCD_DATA_MASK;
  LCD_CTL_PORT &= ~(1<<LCD_EN);
 // set appropriate RS bit 
  if(RS)
    LCD_CTL_PORT |= (1<<LCD_RS);
  else
    LCD_CTL_PORT &= ~(1<<LCD_RS);
  // set RW (read)
  LCD_CTL_PORT |= (1<<LCD_RW);

  // read high nibble
  LCD_CTL_PORT |= (1<<LCD_EN);
 __asm__("nop");
  data |= (LCD_DATA_PIN << 4) & 0xF0;

  LCD_CTL_PORT &= ~(1<<LCD_EN);
 __asm__("nop");

  // read low nibble
  LCD_CTL_PORT |= (1<<LCD_EN);
 __asm__("nop");
  data |= (LCD_DATA_PIN  & 0x0F);

  LCD_CTL_PORT &= ~(1<<LCD_EN);
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
      if((!*data) | (*data == '\n'))
        break;
      lcdWrite(RS_DATA, *data);
      ++data;
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
