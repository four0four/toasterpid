#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "temp.h"
#include "lcd.h"

// 10HZ flag for LCD/serial/thermo/PID
#define _10HZ 0x80

void timerInit(){
  // No, this is not actually pwm. Handling a zero crossing SSR is possible in hardware, but ugly and (in my opinion) less effective.
  // 100 Hz interrupt

  TCCR1B |= (1<<WGM12); // enable CTC
  OCR1A = 1249; // 100Hz at 8MHz, /64
  TIMSK1 |= (1<<OCIE1A); // enable timer1 triggered interrupts
  TCCR1B |= (1<<CS11) | (1<<CS10); // div by 64
}

// count to match ssr output to pid calculation
volatile uint8_t ssrCnt = 0;
// task flags 
volatile uint8_t flags = 0;
// 10hz counter
volatile uint8_t div10count = 0;
// current PID results
volatile uint8_t curPID = 127;


int main() {
  // for debug info via sprintf
  char str[20];
  // last thermo read
  uint32_t lastRead = 0;
  // cludgy startup delay for external hw
  _delay_ms(100);  
  // temp. heartbeat
  DDRD |= (1<<6);

  // init hardware
  serialInit();
  initThermo();
  lcdInit(); 
  timerInit();
   
  serialString("initialization completed.\n\r");

  lcdWrite(RS_CMD, LCD_CLR);
  lastRead = readTemp();

  lcdWriteLine(0, 2, "Thermocouple");

  // good to go:
  sei();

  while(1) {
    if(flags & _10HZ){
      flags &= ~_10HZ;
      lastRead = readTemp();
      // temporary packet start token
      serialWrite(0xAB);
      serialWrite(0xCD);
      sprintf(str," %d \r",getExternalTemp(lastRead));
      serialString(str);
      sprintf(str, "      %d%cC      ", getExternalTemp(lastRead),0xDF);
      if(getFaults(lastRead) == THERM_SCV_FAULT)
        lcdWriteLine(1, 0, "Short to VCC (4)");
      else if(getFaults(lastRead) == THERM_SCG_FAULT)
        lcdWriteLine(1, 0, "Short to GND (2)");
      else if(getFaults(lastRead) == THERM_OC_FAULT)
        lcdWriteLine(1, 0, "Open probe (1)");
      else
        lcdWriteLine(1, 0, str);
    }
  }
}


ISR(TIMER1_COMPA_vect) {
  // update SSR state
  ++ssrCnt;
  if(ssrCnt <= curPID)
    PORTD |= (1<<6);
  else
    PORTD &= ~(1<<6);

  if(div10count == 9){
    div10count = 0;
    flags |= _10HZ;
  }
  else
    ++div10count;
}
