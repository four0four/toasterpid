#include "sdcard.h"


static inline void _sd_send(uint8_t d){
  SPDR = d;
  SDWAITSPI;
}

static inline uint8_t _sd_recv(){
 SDWAITSPI;
 return SPDR;
}

uint8_t sd_init(){
  // TODO
  // set up avr spi here

  uint8_t buf       = 0;
  uint32_t timeout  = 0;
  uint32_t OCR      = 0;
  
  // hold MOSI for > 74 clocks
  for(uint8_t i = 0; i < 10; ++i){
    SPDR = 0xFF;
    SDWAITSPI;
  }

  buf = sd_send_command(CMD0, 0x00000000);

  if(buf != 0x01)
    return buf;

  buf = sd_send_command(CMD8, 0x000001AA);

  if(buf != 0x05) { // 0x05 -> illegal command, therefore we're a SDv1/MMC card
    // IS SDv1/MMC card
    if(buf != 0x01)
      return 0xFF; // ERROR
    else {         // we need to read in OCR
      OCR =  (uint32_t)_sd_recv() << 24;
      OCR |= (uint32_t)_sd_recv() << 16;
      OCR |= (uint32_t)_sd_recv() <<  8;
      OCR |= (uint32_t)_sd_recv();
      if((OCR & 0xFFF) != 0x1AA)
        // ERROR
        return 0xFF;
    }
  }

  // wait for the card to complete internal initialization
  // TODO - tweak timeout value
  while(sd_send_command(CMD1, 0x00) != 0x00){
    if(++timeout > 1000)
      // ERROR
      return 0xFF;
  }

  // attempt to (initially) set sector size to 512B
  buf = sd_send_command(CMD16, 0x200);
  if(buf != 0x00)  // ERROR in buf
    return buf;

  SDSELECT;
  SDUNSELECT;

  return 0; // successful initialization
}


uint8_t sd_send_command(uint8_t cmd, uint32_t arg){
  uint8_t res = 0;
  uint8_t i   = 0;

  SDSELECT;

  // write out command, formatted properly
  _sd_send(cmd | 0x40);
  // write out 32 bits of arg
  _sd_send(arg >> 24); _sd_send(arg >> 16); _sd_send(arg >> 8); _sd_send(arg);

  // hard-coded CRC values for CMD0 and CMD8
  if(cmd == CMD0)
    _sd_send(0x95);
  else if(cmd == CMD8)
    _sd_send(0x87);
  // else we don't care, send 0xFF
  else
    _sd_send(0xFF);

  if((cmd == CMD8) || (cmd == CMD58)) {
     for(i = 0; i < 9; ++i){
      _sd_send(0xFF);
      res = _sd_recv();
      if(res != 0xFF)
        break;
      else continue;
    }
  } else {
    for(i = 0; i < 9; ++i){
      _sd_send(0xFF);
      res = _sd_recv();
      if(res != 0xFF)
        break;
    }
  }
  
  // read off the last trailing bytes - usually CRC we don't care for
  while(_sd_recv() != 0xFF)
    _sd_send(0xFF);

  SDUNSELECT;
  return res;
}

uint8_t sd_read_block(uint8_t *buffer, uint32_t addr){
  uint8_t res = 0;
  uint16_t i  = 0;

  SDSELECT;

  res = sd_send_command(CMD17, addr);
  if(res != 0x00) // ERROR
    return res;

  do
    res = _sd_recv();
  while (res == 0xFF);

  if(res != 0xFE) // invalid data token
    return 0x7f;  // get real errors 

  // read in the sector
  for(i = 0; i < 0x200; ++i){
    _sd_send(0xFF);
    buffer[i] = _sd_recv();
  }

  // clear off any remaining crap
  // this at least purges the CRC bytes
  do
    _sd_send(0xFF);
  while(_sd_recv() != 0xFF);

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

  _sd_send(0xFE);
  _sd_recv();

  for(i = 0; i < 512; ++i){
    _sd_send(buffer[i]);
    _sd_recv();
  }

  // CRC dummy packets
  _sd_send(0xFF);
  _sd_recv();
  _sd_send(0xFF);
  _sd_recv();

  do
    _sd_send(0xFF);
  while(_sd_recv() != 0xFF);

  SDUNSELECT;

  return 0;
}

/*
void    sd_read_multi(uint8_t *buffer, uint32_t addr, uint32_t count){
  return;
}

void    sd_write_multi(uint8_t *buffer, uint32_t addr, uint32_t count){
  return;
}*/
