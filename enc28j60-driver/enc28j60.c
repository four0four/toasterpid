#include "enc28j60.h"

void set_enc_register(uint8_t reg, uint8_t d) {
  select(reg & BANKMASK) {
    case _BANK0:
      // handle bank0 switch
      break;
    case _BANK1:
      // etc.
      break;
    case _BANK2:
      break;
    case _BANK3:
      break;
    default:
      // register is {global, phy}
  }
}


uint8_t get_enc_register(uint8_t reg) {

}
