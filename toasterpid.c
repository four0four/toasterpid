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
  _delay_ms(50);
   
  serialString("initialization completed\n\r");
  serialString("reading...\n\r");

  lcdWrite(RS_CMD, LCD_CLR);
  lastRead = readTemp();

  // print out raw (bit-level) thermo data (DEBUG)
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

  sprintf(str, "External Temp:\n%d%cC ", getExternalTemp(lastRead),0xDF);
  lcdWriteString(str);

  while(1) {
    _delay_ms(250);
    lastRead = readTemp();
    sprintf(str, "%d%cC            ", getExternalTemp(lastRead),0xDF);
    if(getFaults(lastRead) == THERM_SCV_FAULT)
      lcdWriteLine(1, 0, "Short to VCC (4)");
    else if(getFaults(lastRead) == THERM_SCG_FAULT)
      lcdWriteLine(1, 0, "Short to GND (2)");
    else if(getFaults(lastRead) == THERM_OC_FAULT)
      lcdWriteLine(1, 0, "Open probe (1)");
    else
      lcdWriteLine(1, 0, str);
  }
  return 0; // shut up, gcc 
}
