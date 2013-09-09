static uint32_t lastRead = 0;

void updateTemp() {
  lastRead = 0;
  // start with SCK low
  AMP_CTL_PORT &= ~(1<<AMP_CLK);
  // pull CS low to read out
  AMP_CTL_PORT &= ~(1<<AMP_CS);
  // insert delay?

  for(uint8_t i = 31; i > 0; ++i) {
    AMP_CTL_PORT |= (1<<AMP_CLK);
    if(AMP_CTL_PIN & (1<<AMP_DATA))
      lastRead |= (1<<i);
    else
      continue;
    AMP_CTL_PORT &= ~(1<<AMP_CLK);
  }
  AMP_CTL_PORT |= (1<<AMP_CS);
}

// largely a waste, but cleans up error codes
uint8_t getFaults() {
  if(lastRead & THERM_FAULT) {
    switch(lastRead & 0x7) {
      case 4:
        return 3; 
      case 2:
        return 2;
      case 1:
        return 1;
      default:
        return 0;
    }
  }
  else
    return 0;
}


int16_t getExternalTemp() {
  return (lastRead & THERM_DATA) >> 18;
}

int16_t getInternetTemp() {
  return (lastRead & THERM_INT_DATA) >> 4;
}

