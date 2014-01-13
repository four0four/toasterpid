#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "temp.h"
#include "lcd4bit.h"

// 10Hz flag for LCD/thermo/PID
#define _10HZ 0x80
// 2Hz flag for serial CSV data
#define _2HZ 0x40
// maximum number of steps per profile 
#define _MAX_STEPS 8
// cool-enough-to-handle (deg C)
#define _DONE_TEMP 30
// machine states
#define ERROR 0
#define IDLE 1
#define REFLOWING 2
#define COOLING 3
// PID gains
#define IGAIN 150
#define DGAIN 0
#define PGAIN 80
// integrator boundes
#define IMAX 200
#define IMIN -200
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
void updateTarget();
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
volatile uint8_t curPID = 0;
// current time (used to compute desired temp)
volatile uint16_t curTime = 0;


// TODO move most of these into proper scopes
// last error term 
int8_t lastError = 0;
// last integral term
int16_t lastIntegral = 0;
// current reflow oven state
uint8_t curState = IDLE;
// current step, saves time in getTemp(), may be useful elsewhere
uint8_t curStep = 0;
// current temperature read in from thermo
int16_t curTemp = 25;
// current target temperature, stored in 1/512 deg. no signs.
uint32_t curTarget = 50 * 512; 

// struct for profile data
typedef struct {
  uint8_t numSteps; // can be < _MAX_STEPS for a given profile, rest are not stored in EEPROM
  uint16_t stepTime[_MAX_STEPS]; // total time to spend at a given step/rate
  int16_t stepRate[_MAX_STEPS]; // dTemp for a given step, 1/512 deg/sec (>>9)
  uint8_t ceil[_MAX_STEPS]; // TODO REMOVE ME
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
  serialString("[time], [temp], [target], [pidval]\n\r");

  lcdWrite(RS_CMD, LCD_CLR);
  lastRead = readTemp();

  lcdWriteLine(0, 2, "Thermocouple");
  loadProfile();

  // good to go:
  sei();
  curState = REFLOWING;

  while(1) {
    if(flags & _10HZ){
      flags &= ~_10HZ;
      lastRead = readTemp();
      curTemp = getExternalTemp(lastRead);
      updateTarget();
      curPID = getPID(curTemp);
      sprintf(str, "      %d%cC", curTemp,0xDF);
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
      if((curState == COOLING) && (curTemp <= _DONE_TEMP)){
        curState = IDLE;
        lcdWrite(RS_CMD, LCD_CLR); // clear display/buffer
        lcdWriteLine(0, 6, "Done");
        cli();
      }
#ifdef _REALTIME
      // packet start token
      serialWrite(0xAB);
      serialWrite(0xCD);
      sprintf(str," %d %u \r",curTemp, curTime);
      serialString(str);
#endif
#ifdef _CSV
      sprintf(str,"%u, %d, %u, %u \n\r", curTime, curTemp, (uint16_t)(curTarget>>9), curPID);
      serialString(str);
#endif
    }
  }
}

void timerInit() {
  // No, this is not actually pwm. Handling a zero crossing SSR is possible in hardware, but ugly and (in my opinion) less effective.
  // 100 Hz interrupt

  TCCR1B |= (1<<WGM12); // enable CTC
  // 1875 for 12MHz
  OCR1A = 1250; // 100Hz at 8MHz, /64 
  TIMSK1 |= (1<<OCIE1A); // enable timer1 triggered interrupts
  TCCR1B |= (1<<CS11) | (1<<CS10); // div by 64
}

void updateTarget() {
  
  if(curTime > curProfile.stepTime[curStep]){
    ++curStep;
    if(curStep >= curProfile.numSteps){
      curState = COOLING;
      lcdWrite(RS_CMD, LCD_CLR); // clear display/buffer
      lcdWriteLine(0, 4, "Cooling:");
      // This will just force the elements off, fine for now
      curTarget = 25 * 512;
      return;
    }
  }

  if((curState == REFLOWING) && ((curTarget>>9) < curProfile.ceil[curStep]))
    curTarget += curProfile.stepRate[curStep];
}

uint8_t loadProfile() {
  // currently a single profile - Hardcoded crap
  // really could just use a single byte per temp step, max is *usually* < 255
  // TODO - Expand to multiple profiles
  // TODO - Allow for LCD+button profile selection/browsing
  // TODO get profile from EEPROM and stick it into global struct
  // Option: Store profile in target points + elapsed time format,
  //  then convert to internal format here.
  // ceilings added as a temporary ward against clock skew (interal RC osc)
  curProfile.numSteps = 4;
  curProfile.stepRate[0] = 1.3 * 512 / 10 ;
  curProfile.stepTime[0] = 90;
  curProfile.ceil[0] = 117;
  curProfile.stepRate[1] = 0.5 * 512 / 10;
  curProfile.stepTime[1] = 180;
  curProfile.ceil[1] = 162;
  curProfile.stepRate[2] = 1.5 * 512 / 10;
  curProfile.stepTime[2] = 210;
  curProfile.ceil[2] = 207;
  curProfile.stepRate[3] = -1.5 * 512 / 10;
  curProfile.stepTime[3] = 240;
  curProfile.ceil[3] = 255; // clumsy hack for a clumsy hack.
  
  return 0;
}

void saveProfile() {
  // TODO write profile struct into EEPROM
  // TODO add serial interface for profile creation
}

uint8_t getPID(int16_t in) {
  int16_t error = ((curTarget>>9) - curTemp);
  int32_t pterm = PGAIN * error;
  int32_t iterm = 0;
  int32_t dterm = (error - lastError) * DGAIN;

  lastError = error;

  if(lastIntegral + error > IMAX) iterm = IGAIN * IMAX;
  if(lastIntegral + error < IMIN) iterm = IGAIN * IMIN;
  else iterm = IGAIN * (lastIntegral + error);
  
  iterm += pterm + dterm;


  if(iterm >= 255)
    return 255;
  if(iterm <= 0)
    return 0;
  else
    return iterm;
}

ISR(TIMER1_COMPA_vect) {
  // update SSR state
  ++ssrCnt;
  if((ssrCnt <= curPID) && curState == REFLOWING)
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
