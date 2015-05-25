#include <avr/io.h>
#include <stdint.h> 


// MMC command defines
// Borrowed from elm-chan's examples
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

// SD/MMC types
#define VERUNKNOWN 0x00
#define VER1       0x01
#define VER2SD     0x02
#define VER2SDHC   0x03

// SD statuses
#define SD_UNINIT  0x00
#define SD_READY   0x01
#define SD_ERROR   0x02

// SD ports
#define SDCSPORT PORTB
#define SDCSDDR  DDRB
#define SDCSBIT  (1<<2)
#define SPIPORT  PORTB
#define SPIDDR   DDRB

// SD function macros
#define SDUNSELECT (SDCSPORT |= SDCSBIT)
#define SDSELECT   (SDCSPORT &= ~SDCSBIT)
#define SDWAITSPI   while(!(SPSR & (1<<SPIF)))


// TODO - align this
// TODO - also use this after prototyping
struct sdcard {
  uint8_t type;
  uint8_t status;
  uint16_t blocksize;
  
};

/*
** Initialize sd card, filling local data structures.
*/
uint8_t sd_init();

/*
** Send a single sd/mmc command, arguments supplied if applicable.
** sd_send_command(command, argument)
*/
uint8_t sd_send_command(uint8_t, uint32_t);

/*
** sd_read_block(*destBuffer, srcAddr)
*/
uint8_t sd_read_block(uint8_t *, uint32_t);

/*
** sd_write_block(*srcBuffer, destAddr)
*/
uint8_t sd_write_block(uint8_t *, uint32_t);


/*
** sd_read_multi(*destBuffer, srcAddr, readCount)
*/
void    sd_read_multi(uint8_t *, uint32_t, uint32_t);

/*
** sd_write_multi(*srcBuffer, destAddr, writeCount)
*/
void    sd_write_multi(uint8_t *, uint32_t, uint32_t);

