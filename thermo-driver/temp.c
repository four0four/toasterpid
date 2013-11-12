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
  uint32_t d = 0UL;
  // start with SCK low
  AMP_CTL_PORT &= ~(1<<AMP_CLK);
  // assert ~CS to read
  AMP_CTL_PORT &= ~(1<<AMP_CS);
  _delay_us(1);
  // read in four bytes off SPI bus
  for(uint8_t i = 32; i>0; --i) {
    AMP_CTL_PORT &= ~(1<<AMP_CLK);
    _delay_us(1);
    if(AMP_CTL_PIN & (1<<AMP_DATA))
      d |= ((1UL)<<(i-1));            
    AMP_CTL_PORT |= (1<<AMP_CLK);
    _delay_us(1);
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
  d >>= 18UL;
  uint8_t neg = (d & 1<<13UL);
  // operate on positive
  if(neg)
    d = -d; //gcc, please do this right
  // round properly
  if((d & 0x3) > 1){
    d >>= 2;
    d++;
  }
  else
    d >>= 2;

  if(neg) d = -d;

  return d;
}

int16_t getInternalTemp(uint32_t d) {
  d >>= 4;
  uint8_t neg = (d & (1<<11UL));

  if(neg)
    d = -d;
  if((d & 0xF) > 7){
    d >>= 4;
    d++;
  }
  else
    d >>= 4;

  if(neg)
    d = -d;

  return d;
}

