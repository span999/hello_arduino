
///#include <SPI.h>
#include "myCFG.h"


#ifdef _USE_CRC_
#define                 P_CCITT     0x1021
static void             init_crcccitt_tab( void );
static void             init_crcdnp_tab( void );

static int              crc_tabccitt_init       = false;
static unsigned short   crc_tabccitt[256];
#endif


#ifdef _USE_CRC_
unsigned short update_crc_ccitt( unsigned short crc, char c ) {
    unsigned short tmp, short_c;

    short_c  = 0x00ff & (unsigned short) c;
    if ( ! crc_tabccitt_init ) init_crcccitt_tab();
    tmp = (crc >> 8) ^ short_c;
    crc = (crc << 8) ^ crc_tabccitt[tmp];
    return crc;
}  /* update_crc_ccitt */

static void init_crcccitt_tab( void ) {
    int i, j;
    unsigned short crc, c;

    for (i=0; i<256; i++) {
        crc = 0;
        c   = ((unsigned short) i) << 8;
        for (j=0; j<8; j++) {
            if ( (crc ^ c) & 0x8000 ) crc = ( crc << 1 ) ^ P_CCITT;
            else                      crc =   crc << 1;
            c = c << 1;
        }
        crc_tabccitt[i] = crc;
    }
    crc_tabccitt_init = true;
}  /* init_crcccitt_tab */

unsigned short gen_crc_ccitt( unsigned short initSet, unsigned char *pbArray, unsigned int iSize ) {
  unsigned short usRet = 0;
  
  if( pbArray && 0 < iSize ) {
    unsigned int iLoop = 0;
  
    usRet = initSet;
    for ( iLoop = 0; iLoop < iSize; iLoop++ ) {
      usRet = update_crc_ccitt( usRet, pbArray[iLoop] );
    } 
  }
  return usRet;
}
#endif  /* _USE_CRC_ */


/* -------------------------------------------------------------------------------------- */

void myCRC_setup() {
  // put your setup code here, to run once:

}

void myCRC_loop() {
  // put your main code here, to run repeatedly:

}
