#include "lcd4bit.h"

static inline void strobe_en() {
  LCD_CTL_PORT &= ~(1<<LCD_EN);
  LCD_CTL_PORT |= (1<<LCD_EN);
  // One of the following *may* be required at faster clocks:
  //_delay_us(1);
  //__asm__("nop");
  LCD_CTL_PORT &= ~(1<<LCD_EN);
}

static inline void lcd_wait() {
  while(lcd_read_byte(0) & 0x80);
}

void lcd_init() {
  // initialize data pins for 8 bit use
  LCD_CTL_DDR |= (1<<LCD_EN) | (1<<LCD_RS) | (1<<LCD_RW);
  LCD_DATA_DDR |= LCD_DATA_MASK;
  
  // initial pin conditions
  LCD_CTL_PORT |= (1<<LCD_EN);
  LCD_CTL_PORT &= ~(1<<LCD_RS);

  // magic ;)
  // not really, but kinda - ref. datasheets
  lcd_write_byte(0, 0x30);
  _delay_ms(6);
  lcd_write_byte(0, 0x30);
  _delay_ms(2);
  lcd_write_byte(0, 0x30);
  _delay_ms(2);

  // now in 4-bit mode

  // set up 2 lines, 8 bit bus, 5x8 font
  lcd_write_byte(RS_CMD, LCD_FUNCTION | FUNCTION_DISPLAY_LINES);  
  lcd_wait();

  // LCD off (clean reset at least)
  lcd_write_byte(RS_CMD, LCD_POWER); 
  lcd_wait();

  // clear display/buffer
  lcd_write_byte(RS_CMD, LCD_CLR); 
  lcd_wait();

  // set to increment on write
  lcd_write_byte(RS_CMD, LCD_ENTRY | ENTRY_INCREMENT); 
  lcd_wait();

  // basic init done!
  // turn on LCD w/o cursor
  lcd_write_byte(RS_CMD, LCD_POWER | DISPLAY_ON); 
  lcd_wait();
}

void lcd_write_byte(uint8_t RS, uint8_t data) {
  // make sure direction is set
  LCD_CTL_DDR |= ((1<<LCD_EN)|(1<<LCD_RS)|(1<<LCD_RW));
  LCD_DATA_DDR |= LCD_DATA_MASK;

  // clear RW (write)
  LCD_CTL_PORT &= ~(1<<LCD_RW);
  
  if(RS)
    LCD_CTL_PORT |= (1<<LCD_RS);
  else
    LCD_CTL_PORT &= ~(1<<LCD_RS);

  // push first (high) nibble
  LCD_DATA_PORT &= ~LCD_DATA_MASK;
  LCD_DATA_PORT |= (data >> 4);
  strobe_en();

  // push second (low) nibble
  LCD_DATA_PORT &= ~LCD_DATA_MASK;
  LCD_DATA_PORT |= (data & 0x0F);
  strobe_en();

  lcd_wait();
}

uint8_t lcd_read_byte(uint8_t RS) {
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

void lcd_write_char(char data) {
  lcd_write_byte(RS_DATA, data);
}

void lcd_write_str(char *data) {
  uint8_t start_addr = lcd_read_byte(RS_CMD) & 0x7F;
  uint8_t pos;

  for(pos = start_addr; pos < 16; ++pos) {
    lcd_write_byte(RS_DATA, *data);
    ++data;
    if((!*data) | (*data == '\n')) 
      break;
  }
  if(*data) {
    // newline?
    if(*data == '\n')
      ++data;

    // work on line two
    lcd_write_byte(RS_CMD, 0xC0); // DDRAM address to line 2, pos 0
    lcd_wait();
    for(pos = 0; pos < 16; ++pos) {
      if((!*data) | (*data == '\n'))
        break;
      lcd_write_byte(RS_DATA, *data);
      ++data;
    }
  }
}

void lcd_write_line(uint8_t line, uint8_t pos, char *data) {
  if(line == 0)
    lcd_write_byte(RS_CMD, 0x80 + pos);
  else
    lcd_write_byte(RS_CMD, 0xC0 + pos);
  while((*data) && (pos < 16)) {
    lcd_write_byte(RS_DATA, *data);
    ++pos;
    ++data;
  }
}
