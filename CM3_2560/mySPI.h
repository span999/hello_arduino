
#include <SPI.h>


#ifdef _USE_SPI_
void sp_doSPItransfer();
#endif

/* -------------------------------------------------------------------------------------- */
#ifdef _USE_SPI_
void mySPI_setup();
void mySPI_loop();
#endif
