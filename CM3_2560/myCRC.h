
///#include <SPI.h>


#ifdef _USE_CRC_
///void sp_doSPItransfer();
unsigned short update_crc_ccitt( unsigned short crc, char c );
#endif

/* -------------------------------------------------------------------------------------- */
#ifdef _USE_CRC_
void myCRC_setup();
void myCRC_loop();
#endif
