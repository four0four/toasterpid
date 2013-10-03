#include "temp.h"


void initThermo() {
  AMP_CTL_DDR &= ~(1<<AMP_DATA);
  // Pull CS high
  AMP_CTL_DDR |= (1<<AMP_CS);
  AMP_CTL_PORT |= (1<<AMP_CS);
  // Pull SCK low
  AMP_CTL_DDR |= (1<<AMP_CLK);
  AMP_CTL_PORT &= ~(1<<AMP_CLK);
}

uint32_t readTemp() {
  uint32_t d = 0;
  // start with SCK low
  AMP_CTL_PORT &= ~(1<<AMP_CLK);
  // assert ~CS to read
  AMP_CTL_PORT &= ~(1<<AMP_CS);
  // read in four bytes off SPI bus
  for(uint8_t i = 31; i>0; --i) {
    AMP_CTL_PORT &= ~(1<<AMP_CLK);
    if(AMP_CTL_PIN & (1<<AMP_DATA))
      d |= (1<<i);
    _delay_us(5);
    AMP_CTL_PORT |= (1<<AMP_CLK);
  }
  // deassert ~CS
  AMP_CTL_PORT |= (1<<AMP_CS);
  return d;
}

// largely a waste, but cleans up error codes
uint8_t getFaults(uint32_t d) {
  if(d & THERM_FAULT) 
    return d & THERM_FAULT_MASK;
  else
    return 0;
}

int16_t getExternalTemp(uint32_t d) {
  //int16_t extTemp = (d & THERM_DATA) >> 18; // cut out raw value
  //uint8_t neg = (d >> 30) & 0x01; // Save sign bit
  
  // TESTME
  uint16_t extTemp = (int16_t)((d & 0xFFFC0000) >> 20);
  // verify we need this:
  if (~extTemp == 0)
    extTemp = 0;
  return extTemp;
 /*
    d = ((d >> 18) & 0x3fff);   // leave only thermocouple value in d
    if (d & 0x2000)             // if thermocouple reading is negative...
    {
        d = -d & 0x3fff;        // always work with positive values
        neg = 1;             // but note original value was negative
    }
    d = d + 2;                  // round up by 0.5 degC (2 LSBs)
    d = d >> 2;                 // now convert from 0.25 degC units to degC
    if (neg)  d = -d;           // convert to negative if needed
    return  d;                  // return as integer
  */
}

int16_t getInternalTemp(uint32_t d) {
  int16_t intTemp = (int16_t)((d & THERM_INT_DATA) >> 8); // drop from 1/16th precision to 1 deg..

  if (~intTemp == 0)
    intTemp = 0;

  return intTemp;
}

