#include "sdcard.h"
#include "../lcd-driver/lcd4bit.h"

static inline uint8_t _sd_xchg(uint8_t d){
  SPDR = d;
  SDWAITSPI;
  return SPDR;
}

uint8_t sd_init(sdcard *s){
  uint8_t buf       = 0;
  uint32_t timeout  = 0;

  char str[25] = {0};

  s->status = SD_UNINIT;

  // TODO fix this
  // SPI pin directions
  SDCSDDR |= SDCSBIT;
  DDRB |= (1<<3) | (1<<5);
  DDRB &= ~(1<<4);
  // SPI hardware settings
  SPCR = 0x51;

  PORTD |= (1<<6);
  // hold MOSI for > 74 clocks
  for(uint8_t i = 0; i < 10; ++i){
    SPDR = 0xFF;
    SDWAITSPI;
  }

  buf = sd_command(s->r, CMD0, 0x00000000);

  if(buf != 0x01){
    s->status = SD_ERROR;
    return buf;
  }

  buf = sd_command(s->r, CMD8, 0x000001AA);
//  sprintf(str,"buf: 0x%x\n", buf);
//  lcdWriteString(str);

  if(buf != 0x05) { // 0x05 -> illegal command, therefore we're a SDv1/MMC card
    // IS SDv1/MMC card
    if(buf != 0x01){
      s->status = SD_ERROR;
      return 0xFF; 
    }
    else {         // check ocr 

      sprintf(str,"ocr: %08lx", s->r->data);
      lcdWriteString(str);

      while(1);

      if((s->r->data & 0xFFF) != 0x1AA)
        s->status = SD_ERROR;
        return 0xFF;
    }
  }

//  lcdWriteString("asdf");
  // wait for the card to complete internal initialization
  // TODO - tweak timeout value
  while(sd_command(s->r,CMD1, 0x00) != 0x00){
    if(++timeout > 1000){
      lcdWriteString("TIMEOUT");
      while(1);
      // ERROR
      return 0xFF;
    }
  }

  // attempt to (initially) set sector size to 512B
  buf = sd_send_command(CMD16, 0x200);
  if(buf != 0x00)  // ERROR in buf
    return buf;

  SDSELECT;
  SDUNSELECT;

  return 0; // successful initialization
}


// rewrite:
// return error/success (error direct from sd/mmc, success = 0)

uint8_t sd_command(resp *r, uint8_t cmd, uint32_t arg){
  uint8_t i   = 0;
  r->resp = 0;

  SDSELECT;

  // write out command, formatted properly
  _sd_xchg(cmd | 0x40);
  // write out 32 bits of arg
  _sd_xchg((arg >> 24) & 0xFF);
  _sd_xchg((arg >> 16) & 0xFF);
  _sd_xchg((arg >> 8) & 0xFF);
  _sd_xchg(arg & 0xFF);

  // hard-coded CRC values for CMD0 and CMD8
  if(cmd == CMD0)
    _sd_xchg(0x95);
  else if(cmd == CMD8)
    _sd_xchg(0x87);
  // else we don't care, send 0xFF
  else
    _sd_xchg(0xFF);


  for(i = 0; i < 15; ++i){
    r->resp = _sd_xchg(0xFF);
    if(r->resp != 0xFF)
      break;
  }

  // read off OCR/extended response if we need to
  if((cmd == CMD8) || (cmd == CMD58)) {
    r->data  = (_sd_xchg(0xFF)) << 24;
    r->data |= (_sd_xchg(0xFF)) << 16;
    r->data |= (_sd_xchg(0xFF)) <<  8;
    r->data |= (_sd_xchg(0xFF))      ;
  }

  // read off the last trailing bytes - usually CRC we don't care for
  while(_sd_xchg(0xff) != 0xFF);

  SDUNSELECT;
  // Now, this is probably bad/wasteful
  // but it makes some other stuff a helluva lot cleaner
  // TODO think about this with a clear head: can probably just remove it and while(r->resp != result) sd_command(stuff) instead
  return r->resp;
}

uint8_t sd_read_block(uint8_t *buffer, uint32_t addr){
  uint8_t res = 0;
  uint16_t i  = 0;

  SDSELECT;

  res = sd_send_command(CMD17, addr);
  if(res != 0x00) // ERROR
    return res;

  while(res = _sd_xchg(0xFF) == 0xFF);

  if(res != 0xFE) // invalid data token
    return res; 

  // read in the sector
  for(i = 0; i < 0x200; ++i)
    buffer[i] = _sd_xchg(0xFF);

  // clear off any remaining crap
  // this at least purges the CRC bytes
  while(_sd_xchg(0xFF) != 0xFF);

  SDUNSELECT;

  return 0;
}

uint8_t sd_write_block(uint8_t *buffer, uint32_t addr){
  uint8_t res = 0;
  uint16_t i  = 0;

  SDSELECT;

  res = sd_send_command(CMD24, addr);

  if(res != 0x00) // ERROR
    return res;

  _sd_xchg(0xFE);

  for(i = 0; i < 512; ++i)
    _sd_xchg(buffer[i]);

  // CRC dummy packets
  _sd_xchg(0xFF);
  _sd_xchg(0xFF);

  while(_sd_xchg(0xFF) != 0xFF);

  SDUNSELECT;

  return 0;
}

/*
void    sd_read_multi(uint8_t *buffer, uint32_t addr, uint32_t count){
  return;
}

vo!!g
  return;
}*/
