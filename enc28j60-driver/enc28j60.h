#include <stdint.h>
// SPI functions:
#include "serial.h"

// internal register definitions:

// Universal:
#define EIE 0x1B
#define EIR 0x1C
#define ESTAT 0x1D
#define ECON2 0x1E
#define ECON1 0x1F

// bits 7-5 aren't used, so...
#define _BANK0 0x20
#define _BANK1 0x40
#define _BANK2 0x80
#define _BANK3 0xA0
#define BANKMASK 0x1F

// Bank 0:
#define ERDPTL (0x00 | _BANK0)
#define ERDPTH (0x01 | _BANK0)
#define EWRPTL (0x02 | _BANK0)
#define EWRPTH (0x03 | _BANK0)
#define ETXSTL (0x04 | _BANK0)
#define ETXSTH (0x05 | _BANK0)
#define ETXNDL (0x06 | _BANK0)
#define ETXNDH (0x07 | _BANK0)
#define ERXSTL (0x08 | _BANK0)
#define ERXSTH (0x09 | _BANK0)
#define ERXNDL (0x0A | _BANK0)
#define ERXNDH (0x0B | _BANK0)
#define ERXRDPTL (0x0C | _BANK0)
#define ERXRDPTH (0x0D | _BANK0)
#define ERXWRPTL (0x0E | _BANK0)
#define ERXWRPTH (0x0F | _BANK0)
#define EDMASTL (0x10 | _BANK0)
#define EDMASTH (0x11 | _BANK0)
#define EDMANDL (0x12 | _BANK0)
#define EDMANDH (0x13 | _BANK0)
#define EDMADSTL (0x14 | _BANK0)
#define EDMADSTH (0x15 | _BANK0)
#define EDMACSL (0x16 | _BANK0)
#define EDMACSH (0x17 | _BANK0)

// Bank 1:
#define EHT0 (0x00 | _BANK1)
#define EHT1 (0x01 | _BANK1)
#define EHT2 (0x02 | _BANK1)
#define EHT3 (0x03 | _BANK1)
#define EHT4 (0x04 | _BANK1)
#define EHT5 (0x05 | _BANK1)
#define EHT6 (0x06 | _BANK1)
#define EHT7 (0x07 | _BANK1)
#define EPMM0 (0x08 | _BANK1)
#define EPMM1 (0x09 | _BANK1)
#define EPMM2 (0x0A | _BANK1)
#define EPMM3 (0x0B | _BANK1)
#define EPMM4 (0x0C | _BANK1)
#define EPMM5 (0x0D | _BANK1)
#define EPMM6 (0x0E | _BANK1)
#define EPMM7 (0x0F | _BANK1)
#define EPMCSL (0x10 | _BANK1)
#define EPMCSH (0x11 | _BANK1)
#define EPMOL (0x14 | _BANK1)
#define EPMOH (0x15 | _BANK1)
#define EWOLIE (0x16 | _BANK1)
#define EWOLIR (0x17 | _BANK1)
#define ERXFCON (0x18 | _BANK1)
#define EPKTCNT (0x19 | _BANK1)

// Bank 2:
#define MACON1 (0x00 | _BANK2)
#define MACON2 (0x01 | _BANK2)
#define MACON3 (0x02 | _BANK2)
#define MACON4 (0x03 | _BANK2)
#define MABBIPG (0x04 | _BANK2)
#define MAIPGL (0x06 | _BANK2)
#define MAIPGH (0x07 | _BANK2)
#define MACLCON1 (0x08 | _BANK2)
#define MACLCON2 (0x09 | _BANK2)
#define MAMXFLL (0x0A | _BANK2)
#define MAMXFLH (0x0B | _BANK2)
#define MAPHSUP (0x0D | _BANK2)
#define MICON (0x11 | _BANK2)
#define MICMD (0x12 | _BANK2)
#define MIREGADR (0x14 | _BANK2)
#define MIWRL (0x16 | _BANK2)
#define MIWRH (0x17 | _BANK2)
#define MIRDL (0x18 | _BANK2)
#define MIRDH (0x19 | _BANK2)

// Bank 3:
#define MAADR1 (0x00 | _BANK3)
#define MAADR0 (0x01 | _BANK3)
#define MAADR3 (0x02 | _BANK3)
#define MAADR2 (0x03 | _BANK3)
#define MAADR5 (0x04 | _BANK3)
#define MAADR4 (0x05 | _BANK3)
#define EBSTSD (0x06 | _BANK3)
#define EBSTCON (0x07 | _BANK3)
#define EBSTCSL (0x08 | _BANK3)
#define EBSTCSH (0x09 | _BANK3)
#define MISTAT (0x0A | _BANK3)
#define EREVID (0x12 | _BANK3)
#define ECOCON (0x15 | _BANK3)
#define EFLOCON (0x17 | _BANK3)
#define EPAUSL (0x18 | _BANK3)
#define EPAUSH (0x19 | _BANK3)

// PHY register definitions
#define PHCON1 0x00
#define PHSTAT1 0x01
#define PHID1 0x02
#define PHID2 0x03
#define PHCON2 0x10
#define PHSTAT2 0x11
#define PHIE 0x12
#define PHIR 0x13
#define PHLCON 0x14

// PHY SPI opcodes
#define RCR 0x0
#define RBM 0x1
#define WCR 0x2
#define WBM 0x3
#define BFS 0x4
#define BFC 0x5
#define SC  0x7


// general macros
#define enc_ctl_port  PORTD
#define enc_ctl_ddr   DDRD
#define enc_cs_pin    4
#define assert_enc_cs   enc_ctl_port &= ~(1<<enc_cs_pin) 
#define deassert_enc_cs enc_ctl_port |=  (1<<enc_cs_pin)


/*
  get/set enc registers - detect and set bank
  no bank set if: global control register /or/ PHY register
    there are no collisions.
*/
void set_enc_register(uint8_t reg, uint8_t d);
uint8_t get_enc_register(uint8_t reg);

/*
  get/set phy registers
void setEncPhyRegister(uint16_t reg, uint16_t d);
uint16_t getEncPhyRegister(uint16_t reg);
*/

// push len bytes from *data to the phy
void write_enc_mem(uint8_t *data, uint16_t len);

// array must be allocated and <= len
void read_enc_mem(uint8_t *data, uint16_t len);

//void setEncBit(
