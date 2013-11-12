#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "temp.h"
#include "lcd.h"


int main() {
  // for debug info via sprintf
  char str[20];
  // last thermo read
  uint32_t lastRead = 0;
  // crappy 4-bit spacing counter
  uint8_t sp = 0;
  // cludgy startup delay for external hw
  _delay_ms(100);  

  // init hardware
  serialInit();
  initThermo();
  lcdInit(); 
  serialString("initialization completed\n\r");

  _delay_ms(50);

  serialString("reading...\n\r");
  lastRead = readTemp();

  // print out raw (bit-level) thermo data
  for(uint8_t i = 32; i > 0; --i) {
    if(sp==4){
      serialWrite(' ');
      sp = 0;
    }
    sp++;
    if(lastRead & (1UL<<(i-1)))
      serialWrite('1');
    else
      serialWrite('0');
  }
  serialWrite('\n');
  serialWrite('\r');

  sprintf(str, "Ext. Temp: %d\n\r", getExternalTemp(lastRead));
  serialString(str);
  sprintf(str, "Int. Temp: %d\n\r", getInternalTemp(lastRead));
  serialString(str);

  sprintf(str,"Error code: 0x%02x\n\r",getFaults(lastRead));
  serialString(str);

  sprintf(str, "Ext. Temp: %d%cC ", getExternalTemp(lastRead),0xDF);

  int i = 0;
  lcdWriteString(str);
  /*
  while(str[i]) {
    lcdWrite(1, str[i]);
    ++i;
  }
  */
  return 0; // shut up, gcc 
}
