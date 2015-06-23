
///#include <SPI.h>


#ifdef _USE_CM3_
void doCMstate( unsigned int systick );
void doCmd( int iCID );
void doCM3parsing( unsigned char ucByteIn );

extern unsigned int authorize;
#endif

/* -------------------------------------------------------------------------------------- */
#ifdef _USE_CM3_
void myCM3_setup();
void myCM3_loop();
#endif
