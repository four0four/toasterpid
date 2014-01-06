#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "temp.h"
#include "lcd.h"

// 10Hz flag for LCD/thermo/PID
#define _10HZ 0x80
// 2Hz flag for serial CSV data
#define _2HZ 0x40
// maximum number of steps per profile 
#define _MAX_STEPS 8
// name length per profile
// #define _NAME_LEN 8
// debug options
//#define _REALTIME
#define _CSV

// set up timer interrupt hw
void timerInit();
// load (currently the only) profile from EEPROM to RAM globals
uint8_t loadProfile();
// save (currently the only) profile to EEPROM
void saveProfile();
// Interpolate stored profile values
uint16_t getTemp(uint16_t time);
// return 0-255 from PID algo
uint8_t getPID(int16_t in);

// count to match ssr output to pid calculation
volatile uint8_t ssrCnt = 0;
// task flags 
volatile uint8_t flags = 0;
// 10hz counter
volatile uint8_t div10count = 0;
// 2hz counter
volatile uint8_t div50count = 0;
// 1hz counter
volatile uint8_t div100count = 0;
// current PID results
volatile uint8_t curPID = 127;
// current time (used to compute desired temp)
volatile uint16_t curTime = 0;


// current step, saves time in getTemp(), may be useful elsewhere
uint8_t curStep = 0;
// current slope (saves on continuous interpolation)
float curSlope = 0;

// struct for profile data
typedef struct prof {
  uint8_t numSteps; // can be < _MAX_STEPS for a given profile, rest are not stored in EEPROM
  int16_t tempStep[_MAX_STEPS]; // temperature at a given step
  uint16_t timeStep[_MAX_STEPS]; // time for a given temp step
#ifdef _NAME_LEN
  uint8_t name[_NAME_LEN];
#endif
} profile;

profile curProfile;

int main() {
  // for debug info via sprintf
  char str[20];
  // last thermo read
  uint32_t lastRead = 0;
  // calculated temp
  int16_t curTemp = 25;
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
  serialString("[time], [temp], [target]\n\r");

  lcdWrite(RS_CMD, LCD_CLR);
  lastRead = readTemp();

  lcdWriteLine(0, 2, "Thermocouple");
  loadProfile();

  // good to go:
  sei();

  while(1) {
    if(flags & _10HZ){
      flags &= ~_10HZ;
      lastRead = readTemp();
      curTemp = getExternalTemp(lastRead);
      curPID = getPID(curTemp);
      sprintf(str, "      %d%cC      ", curTemp,0xDF);
      if(getFaults(lastRead) == THERM_SCV_FAULT)
        lcdWriteLine(1, 0, "Short to VCC (4)");
      else if(getFaults(lastRead) == THERM_SCG_FAULT)
        lcdWriteLine(1, 0, "Short to GND (2)");
      else if(getFaults(lastRead) == THERM_OC_FAULT)
        lcdWriteLine(1, 0, "Open probe (1)");
      else
        lcdWriteLine(1, 0, str);
    }
    if(flags & _2HZ){
      flags &= ~_2HZ;
#ifdef _REALTIME
      // packet start token
      serialWrite(0xAB);
      serialWrite(0xCD);
      sprintf(str," %d %u \r",curTemp, curTime);
      serialString(str);
#endif
#ifdef _CSV
      sprintf(str,"%u, %d, %u \n\r", curTime, curTemp, getTemp(curTime));
      serialString(str);
//      sprintf(str, "cur slope = %f\n\r", curSlope);
      serialString(str);
#endif
    }
  }
}

void timerInit() {
  // No, this is not actually pwm. Handling a zero crossing SSR is possible in hardware, but ugly and (in my opinion) less effective.
  // 100 Hz interrupt

  TCCR1B |= (1<<WGM12); // enable CTC
  OCR1A = 1249; // 100Hz at 8MHz, /64
  TIMSK1 |= (1<<OCIE1A); // enable timer1 triggered interrupts
  TCCR1B |= (1<<CS11) | (1<<CS10); // div by 64
}

uint16_t getTemp(uint16_t time) {
  // TODO this
  if(curStep == 0){
    curSlope = (curProfile.tempStep[curStep+1] - curProfile.tempStep[curStep]);
    curSlope /= (curProfile.timeStep[curStep+1] - curProfile.timeStep[curStep]);
  }
  if((curTime >= curProfile.timeStep[curStep+1]) && (curStep < curProfile.numSteps)) {
    curStep++;
    // compute slope between current and next steps
    curSlope = (curProfile.tempStep[curStep+1] - curProfile.tempStep[curStep]);
    curSlope /= (curProfile.timeStep[curStep+1] - curProfile.timeStep[curStep]);
  }

  return (curSlope * (time - curProfile.timeStep[curStep])) + curProfile.tempStep[curStep];
}

uint8_t loadProfile() {
  // currently a single profile - Hardcoded crap
  // really could just use a single byte per temp step, max is *usually* < 255
  // TODO - Expand to multiple profiles
  // TODO - Allow for LCD+button profile selection/browsing
  // TODO get profile from EEPROM and stick it into global struct
  
  curProfile.numSteps = 7;
  curProfile.tempStep[0] = 25;
  curProfile.timeStep[0] = 0;
  curProfile.tempStep[1] = 120;
  curProfile.timeStep[1] = 60;
  curProfile.tempStep[2] = 150;
  curProfile.timeStep[2] = 120;
  curProfile.tempStep[3] = 185;
  curProfile.timeStep[3] = 180;
  curProfile.tempStep[4] = 220;
  curProfile.timeStep[4] = 210;
  curProfile.tempStep[5] = 180;
  curProfile.timeStep[5] = 235;
  curProfile.tempStep[6] = 25;
  curProfile.timeStep[6] = 400;
  
  return 0;
}

void saveProfile() {
  // TODO write profile struct into EEPROM
  // TODO add serial interface for profile creation
}

uint8_t getPID(int16_t in) {
  // TODO this  
  return 127;
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
  if(div50count == 49){
    div50count = 0;
    flags |= _2HZ;
  }
  else
    ++div50count;
  if(div100count == 99){
    div100count = 0;
    ++curTime;
  }
  else
    ++div100count;
}
