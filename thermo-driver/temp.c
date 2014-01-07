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
  _delay_us(5);
  // read in four bytes off SPI bus
  for(uint8_t i = 32; i>0; --i) {
    AMP_CTL_PORT &= ~(1<<AMP_CLK);
    _delay_us(15);
    if(AMP_CTL_PIN & (1<<AMP_DATA))
      d |= ((1UL)<<(i-1));            
    AMP_CTL_PORT |= (1<<AMP_CLK);
    _delay_us(10);
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
  uint8_t neg = 0;
  int16_t t = d >> 18UL;

  if(t & (1<<13)) {
    neg = 1;
    --t;
    t = ~t;
    serialString("Found negative temp\n\r");
  }

  t += 2; // Rounds >= 0.5 up
  t >>= 2; // drop 0.25 and 0.5 bits

  if(neg)
    t = -1 * t;

  return t;
  /*d >>= 18UL;
  uint8_t neg = (d & 1<<13UL);
  // operate on positive
  if(neg)
    d *= -1; //gcc, please do this right
  // round properly
  if((d & 0x3) > 1){
    d >>= 2;
    d++;
  }
  else
    d >>= 2;

  if(neg) d *= -1;  */


//  return d >> 18;
}

int16_t getInternalTemp(uint32_t d) {
  int16_t t = d >> 4;

  if(t & (1<<11))
    return 0;
  t += 4; 
  t = t >> 4;
  return d;
}

