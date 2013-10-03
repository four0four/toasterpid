#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "temp.h"
#include "lcd.h"


int main() {
  char str[20];
  uint32_t lastRead = 0;

  serialInit();
  initThermo();

  serialString("initialization completed\n\r");


  _delay_ms(50);

  serialString("reading...\n\r");
  lastRead = readTemp();
  /*
  // pull CS low to read out
  AMP_CTL_PORT &= ~(1<<AMP_CS);

  lastRead = 0;
  for(uint8_t i = 31; i>0; --i) {
    AMP_CTL_PORT &= ~(1<<AMP_CLK);
    if(AMP_CTL_PIN & (1<<AMP_DATA))
      lastRead |= (1<<i);
    _delay_us(5);
    AMP_CTL_PORT |= (1<<AMP_CLK);
  }
  AMP_CTL_PORT |= (1<<AMP_CS);

  serialWrite('\n');
  serialWrite('\r');
  */

  uint16_t upper,lower;
  upper = lastRead >> 16;
  lower = lastRead & 0xFFFF;

  sprintf(str,"Raw: 0x%x%x\n\r",upper, lower);
  serialString(str);

  sprintf(str, "Fault Code: 0x%x\n\r", getFaults(lastRead));
  serialString(str);
  sprintf(str, "Ext. Temp: %d\n\r", getExternalTemp(lastRead));
  serialString(str);
  sprintf(str, "Int. Temp: %d\n\r", getInternalTemp(lastRead));
  serialString(str);


  return 0; // shut up, gcc 
}
