/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

#ifdef __AVR__
#include <stdarg.h>
///#include <MsTimer2.h>
#include <Timer.h>
#else
#include "mbed.h"
#include "rtos.h"
#endif

struct s_dataBuffer {
    unsigned int bufSize;
    unsigned char *pbufArray;
    unsigned int bufHeadCnt;
    unsigned int bufTailCnt;
    unsigned int tagStartCnt;
    unsigned char chkSum;
    unsigned char isPackage;
    unsigned int bufAvalCnt;
};


void removehead2dataBuffer( struct s_dataBuffer *pDataBuf, unsigned char *pBuf );
void reset2dataBuffer( struct s_dataBuffer *pDataBuf );

unsigned char rawdataBuffer[256];
struct s_dataBuffer rawDataBuf = { 256, rawdataBuffer, 0, 0, 0x0FFF, 0, 0, 0 };

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
Timer ledTimer;
Timer datainTimer;
Timer datacheckTimer;
Timer dataparseTimer;

void spDebug(char *fmt, ... ) {
  char buf[128]; // resulting string limited to 128 chars
  
  va_list args;
  va_start (args, fmt );
  
#ifdef __AVR__
  vsnprintf(buf, 128, fmt, args);
  ///vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
#else
  vsnprintf(buf, sizeof(buf), (const char *)fmt, args); // for the rest of the world
#endif
  va_end (args);
  Serial.print(buf);
}

void ledBlink2() {
  static unsigned int uiCnt = 0;

  digitalWrite(led, (0==uiCnt%2)?HIGH:LOW);   // turn the LED on / off
  uiCnt++;
}


unsigned int getheadtailDiff( unsigned int uiHead, unsigned int uiTail ) {
    unsigned int uiRet;
    
    if ( uiHead <= uiTail ) {
        uiRet = uiTail - uiHead;
    } else {
        uiRet = 256 - uiHead + uiTail;
    }
    
    return uiRet;
}

unsigned int getnextIndex( unsigned int uiIdx ) {
    unsigned int uiRet = 0;
    
    uiRet = (uiIdx + 1) & 0xFF;
    
    return uiRet;
}

unsigned int getnextNIndex( unsigned int uiIdx, unsigned int uiCnt ) {
    unsigned int uiRet = 0;
    unsigned int uiTmp = 0;
    
    for ( uiTmp = 0; uiTmp < uiCnt; uiTmp++ ) {
        uiIdx = (uiIdx + 1) & 0xFF;
    }
    
    uiRet = uiIdx;
    return uiRet;
}

unsigned char getChecksum( unsigned char *pCharBuf, unsigned int uiHead, unsigned int uiCnt ) {
    unsigned char ucRet = 0;
    
    if( pCharBuf && uiCnt > 0 ) {
        unsigned int uiTmp = 0;
        unsigned int uiIdx = 0;
        
        uiIdx = uiHead;
        for ( uiTmp = 0; uiTmp < uiCnt; uiTmp++ ) {
            ucRet = ucRet + pCharBuf[uiIdx];
            uiIdx = getnextNIndex(uiIdx,1);
        }        
    } 

    return ucRet;
}

unsigned char getchecksum2dataBuffer( struct s_dataBuffer *pDataBuf, unsigned int uiHead ) {
    unsigned char ucRet = 0;
    
    if ( pDataBuf && (0xFF == pDataBuf->pbufArray[uiHead]) && (0xFF == pDataBuf->pbufArray[getnextIndex(uiHead)]) ) {
        unsigned int uiCnt = 0;
        
        uiCnt = pDataBuf->pbufArray[getnextNIndex(uiHead,2)] + 3;        
        ucRet = getChecksum(pDataBuf->pbufArray, uiHead, uiCnt);
    }
    
    return ucRet;
}

int checkhead2dataBuffer( struct s_dataBuffer *pDataBuf ) {
    int iRet = 0;
    
    if ( pDataBuf ) {
        unsigned int idx;
        unsigned int ihead;
        
        if ( 0 < pDataBuf->isPackage || 0 == pDataBuf->bufAvalCnt )
            return iRet;
        
        ihead = pDataBuf->bufHeadCnt;
        idx = pDataBuf->bufTailCnt;

        ///check 0xFF,0xFF package header
        if ( (0x0FFF == pDataBuf->tagStartCnt) ) {                              ///if package header not found
            if ( 2 <= getheadtailDiff(ihead, idx) ) {
                if ( (0xFF == pDataBuf->pbufArray[ihead]) && (0xFF == pDataBuf->pbufArray[getnextNIndex(ihead,1)]) ) {
                    if ( 124 >= pDataBuf->pbufArray[getnextNIndex(ihead,2)] ) { ///max package size
                        pDataBuf->tagStartCnt = ihead;                          ///package header found!
                        printf("package header found....[%d][%d][%d]\r\n", ihead, idx, pDataBuf->pbufArray[getnextNIndex(ihead,2)]);
                    } else {
                        printf("wrong package size....[%d][%d]\r\n", ihead, idx);
                        removehead2dataBuffer(&rawDataBuf, 0);
                        iRet = (-1);
                        reset2dataBuffer(&rawDataBuf);
                    }
                } else {
                    printf("wrong package 0xFF 0xFF....[%d][%d]\r\n", ihead, idx);
                    removehead2dataBuffer(&rawDataBuf, 0);
                    iRet = (-1);
                }
            } else {
                printf("package data less then 2....[%d][%d]\r\n", ihead, idx);
                ///pDataBuf->bufHeadCnt++;
            }
        } else {///if package header aleady found
            unsigned int uiLoop = 0;
            unsigned int uiSize = 0;
            unsigned char ucSum = 0;
            uiSize = pDataBuf->pbufArray[getnextNIndex(ihead,2)];
            if ( uiSize == getheadtailDiff(ihead, idx)-3-1 ) {   ///checksum time~
                #if 1
                ucSum = getchecksum2dataBuffer( pDataBuf, ihead );
                #else 
                for ( uiLoop = 0; uiLoop < uiSize+3; uiLoop++ ) {
                    ucSum = ucSum + pDataBuf->pbufArray[ihead+uiLoop];
                    #if 0
                    printf("0x%02x-", pDataBuf->pbufArray[ihead+uiLoop]);
                    if( 0 == (uiLoop+1)%10 )
                        printf("\r\n");
                    #endif
                }
                #endif
                
                if ( ucSum == pDataBuf->pbufArray[getnextNIndex(ihead,uiSize+3)] ) {
                    pDataBuf->isPackage = 1;
                    printf("checksum ok....[0x%02x][0x%02x][%d][%d][%d][%d]\r\n", ucSum, pDataBuf->pbufArray[getnextNIndex(ihead,uiSize+3)], uiSize, getheadtailDiff(ihead, idx), ihead, idx);
                } else {
                    pDataBuf->tagStartCnt = 0x0FFF;
                    printf("checksum fail..[0x%02x][0x%02x][%d][%d][%d][%d]\r\n", ucSum, pDataBuf->pbufArray[getnextNIndex(ihead,uiSize+3)], uiSize, getheadtailDiff(ihead, idx), ihead, idx);
                    removehead2dataBuffer(&rawDataBuf, 0);
                    iRet = (-1);
                    
                    printf("\r\n");
                    for ( uiLoop = 0; uiLoop < 128; uiLoop++ ) {
                        printf("0x%02x-", pDataBuf->pbufArray[getnextNIndex(ihead,uiLoop)]);
                        if( 0 == (uiLoop+1)%10 )
                            printf("\r\n");
                    }
                    printf("\r\n");
                    
                }
            } else {
                if ( uiSize > getheadtailDiff(ihead, idx)-3-1 ) {
                    printf("no checksum, maybe next time....[%d][%d][%d][%d]\r\n", uiSize, getheadtailDiff(ihead, idx), ihead, idx );
                } else {
                    printf("wrong pattern size, maybe next time....[%d][%d][%d][%d]\r\n", uiSize, getheadtailDiff(ihead, idx), ihead, idx );
                    removehead2dataBuffer(&rawDataBuf, 0);
                    iRet = (-1);
                    reset2dataBuffer(&rawDataBuf);
                }
            }
        }
        ///pDataBuf->bufHeadCnt++;
        #if 0
        if( pDataBuf->bufSize == pDataBuf->bufHeadCnt ) {
            pDataBuf->bufHeadCnt = 0;
        }
        #else
        pDataBuf->bufHeadCnt = pDataBuf->bufHeadCnt & 0xFF;
        #endif

    }
    
    return iRet;
}

void addtail2dataBuffer( struct s_dataBuffer *pDataBuf, unsigned char Buf ) {
    if ( pDataBuf ) {
        unsigned int idx;
        
        idx = pDataBuf->bufTailCnt;
        pDataBuf->pbufArray[idx] = Buf;
        pDataBuf->bufTailCnt++;
        pDataBuf->bufAvalCnt++;
        #if 0
        if( pDataBuf->bufSize == pDataBuf->bufTailCnt ) {
            pDataBuf->bufTailCnt = 0;
        }
        #else
        pDataBuf->bufTailCnt = pDataBuf->bufTailCnt & 0xFF;
        #endif
    }
}

void removehead2dataBuffer( struct s_dataBuffer *pDataBuf, unsigned char *pBuf ) {
    if ( pDataBuf ) {
        if( 0 == pBuf ) {
        } else {
            *pBuf = pDataBuf->pbufArray[pDataBuf->bufHeadCnt];
        }
        pDataBuf->bufHeadCnt++;
        pDataBuf->bufAvalCnt--;
        #if 0
        if( pDataBuf->bufSize == pDataBuf->bufHeadCnt ) {
            pDataBuf->bufHeadCnt = 0;
        }
        #else
        pDataBuf->bufHeadCnt = pDataBuf->bufHeadCnt & 0xFF;
        #endif
    }
}


unsigned int getdiff2dataBuffer( struct s_dataBuffer *pDataBuf ) {
    unsigned int uiRet = 0;
    if ( pDataBuf ) {
        if( 0xFFF != pDataBuf->tagStartCnt ) {
            uiRet = pDataBuf->bufTailCnt - pDataBuf->bufHeadCnt;
        }
    }
    
    return uiRet;
}

unsigned int gethead2dataBuffer( struct s_dataBuffer *pDataBuf ) {
    unsigned int uiRet = 0;
    if ( pDataBuf ) {
        uiRet = pDataBuf->bufHeadCnt;
    }
    return uiRet;
}

unsigned int gettail2dataBuffer( struct s_dataBuffer *pDataBuf ) {
    unsigned int uiRet = 0;
    if ( pDataBuf ) {
        uiRet = pDataBuf->bufTailCnt;
    }
    return uiRet;
}

unsigned int getstart2dataBuffer( struct s_dataBuffer *pDataBuf ) {
    unsigned int uiRet = 0;
    if ( pDataBuf ) {
        uiRet = pDataBuf->tagStartCnt;
    }
    return uiRet;
}

unsigned int getavaliabe2dataBuffer( struct s_dataBuffer *pDataBuf ) {
    unsigned int uiRet = 0;
    if ( pDataBuf ) {
        uiRet = pDataBuf->bufAvalCnt;
    }
    return uiRet;
}

unsigned char ispack2dataBuffer( struct s_dataBuffer *pDataBuf ) {
    unsigned char ucRet = 0;
    if ( pDataBuf ) {
        ucRet = pDataBuf->isPackage;
    }
    return ucRet;
}

void reset2dataBuffer( struct s_dataBuffer *pDataBuf ) {
    if ( pDataBuf ) {
        pDataBuf->tagStartCnt = 0xFFF;
        pDataBuf->chkSum = 0;
        pDataBuf->isPackage = 0;
        ///pDataBuf->bufHeadCnt = 0;
    }
}

static unsigned char bPattern1[] = {
    0xFF, 0xFF, 0x7C, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
    0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24,
    0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e,
    0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42,
    0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c,
    0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56,
    0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
    0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74,
    0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x00
};  ///127 bytes = 0xFF, 0xFF + 0x7C + 124 bytes data


void updatePattern() {
    static unsigned char b004 = 0;      ///ECU connetion status
    static short s005 = 0;              ///TPS[5-6] status
    static unsigned char b014 = 0;      ///votage status
    static short s015 = 0;              ///rpm[15-16] status
    static unsigned char b127 = 0;      ///variable

    b004++;
    s005++;
    b014++;
    s015++;
    b127++;
    bPattern1[0] = 0xFF;
    bPattern1[1] = 0xFF;
    bPattern1[2] = 0x7C;
    bPattern1[3] = (b004 % 2);
    bPattern1[4] = (s005 >> 8) & 0xFF;
    bPattern1[5] = (s005 >> 0) & 0xFF;
    bPattern1[13] = b014;
    bPattern1[14] = (s015 >> 8) & 0xFF;
    bPattern1[15] = (s015 >> 0) & 0xFF;
    bPattern1[126] = b127;
}

void killPattern() {
    unsigned char ucTmp = 0;

    ucTmp = (unsigned char)(rand()&0xEF);
    bPattern1[0] = ucTmp;
    ucTmp = (unsigned char)(rand()&0xEF);
    bPattern1[1] = ucTmp;
    ucTmp = (unsigned char)(rand()&0xEF);
    bPattern1[2] = ucTmp;
    
    ucTmp = (unsigned char)(rand()&0x7F);
    bPattern1[ucTmp] = 0xFF;
    ucTmp = (unsigned char)(rand()&0x7F);
    bPattern1[ucTmp] = 0xFF;
    ucTmp = (unsigned char)(rand()&0x7F);
    bPattern1[ucTmp] = 0xFF;
    
}

#ifdef __AVR__
void dataIn_thread(void) {
#else
void dataIn_thread(void const *args) {
#endif
    unsigned char bChkSum = 0;
    unsigned int uiLoop = 0;
    unsigned int uiCnt = 0;
    unsigned int uiPatternSize = 0;
    unsigned char *pbPattern = 0;

#ifdef __AVR__
    if(1) {
#else
    while(1) {
#endif      
        uiCnt++;
       
        printf(">>> dataIn while....[%d]\r\n", uiCnt);
        
        if( 11 == uiCnt%9 ) {
            killPattern();
            pbPattern = bPattern1;
            ///uiPatternSize = 127;
            ///uiPatternSize = (unsigned int)(rand() & 0x7F);
            ///uiPatternSize = (unsigned int)(rand() & 0x3F);
            uiPatternSize = (unsigned int)(rand() & 0x1F);
            ///uiPatternSize = (unsigned int)(rand() & 0x0F);
        } else {
            updatePattern();
            pbPattern = bPattern1;
            uiPatternSize = 127;
        }
        
        #if 1
        bChkSum = getChecksum(pbPattern, 0, uiPatternSize);
        #else
        for ( uiLoop = 0; uiLoop < uiPatternSize; uiLoop++ ) {
            bChkSum = bChkSum + pbPattern[uiLoop];
        }
        #endif
        
        ///mySerial.write(bPattern1, 127);
        ///mySerial.write(bChkSum);
        for ( uiLoop = 0; uiLoop < uiPatternSize; uiLoop++ ) {
            addtail2dataBuffer(&rawDataBuf, pbPattern[uiLoop]);
        }
        addtail2dataBuffer(&rawDataBuf, bChkSum);

        printf(">>> dataIn while....size[%d]checksum[0x%02x]\r\n", uiPatternSize, bChkSum);

#ifdef __AVR__
#else
        Thread::wait(300);
        ///Thread::wait(600);
#endif
    }
}

#ifdef __AVR__
void dataCheck_thread(void) {
#else
void dataCheck_thread(void const *args) {
#endif
    unsigned int uiCnt = 0;
    unsigned int uiAva = 0;
    int iRet = 0;

#ifdef __AVR__
    if(1) {
#else
    while(1) {
#endif
        uiCnt++;
        uiAva = getavaliabe2dataBuffer(&rawDataBuf);
        
        if ( 0 == uiAva ) {
#ifdef __AVR__
            return;
#else
            Thread::wait(100);
            continue;
#endif
        }
        
        printf("=== dataCheck while....[%d]available[%d]\r\n", uiCnt, uiAva );
        
        iRet = checkhead2dataBuffer(&rawDataBuf);

        if( 0 == iRet ) {
#ifdef __AVR__
#else
            Thread::wait(50);
        } else {
            Thread::wait(1);
#endif
        }
        ///Thread::wait(100);
    }
}

#ifdef __AVR__
void dataParse_thread(void) {
#else
void dataParse_thread(void const *args) {
#endif
    unsigned int uiRet = 0;
    unsigned int uiHead = 0;
    unsigned int uiTail = 0;
    unsigned int uiStart = 0;
    unsigned char ucRet = 0;
    unsigned char bPack[128];
    unsigned int iLoop = 0;

#ifdef __AVR__
    if(1) {
#else
    while(1) {
#endif
        uiHead = gethead2dataBuffer(&rawDataBuf);
        uiTail = gettail2dataBuffer(&rawDataBuf);
        uiStart = getstart2dataBuffer(&rawDataBuf);
        uiRet = getdiff2dataBuffer(&rawDataBuf);
        ucRet = ispack2dataBuffer(&rawDataBuf);
        iLoop = getavaliabe2dataBuffer(&rawDataBuf);
        
        printf("<<< dataParse while....[%d][%d][0x%04x]diff[%d]pack[%d]ava[%d]\r\n", uiHead, uiTail, uiStart, uiRet, ucRet, iLoop);
        
        if( ucRet > 0 ) {
            
            for ( iLoop = 0; iLoop < 128; iLoop++ ) {
                removehead2dataBuffer(&rawDataBuf, &bPack[iLoop]);
            }
            reset2dataBuffer(&rawDataBuf);
            
            #if 0
            printf("\r\n");
            for ( iLoop = 0; iLoop < 128; iLoop++ ) {
                printf("0x%02x ", bPack[iLoop]);
                if( 0 == (iLoop+1)%10 )
                    printf("\r\n");
            }
            printf("\r\n");
            #endif
            
            printf("[0x%02x][0x%02x][0x%02x]\r\n", bPack[0], bPack[1], bPack[2] );
            printf("ECU=[0x%02x],TPS=[0x%02x][0x%02x]\r\n", bPack[3], bPack[4], bPack[5] );
            printf("Voltage=[0x%02x],RPM=[0x%02x][0x%02x]\r\n", bPack[13], bPack[14], bPack[15] );
            printf("variable=[0x%02x],checksum=[0x%02x]\r\n", bPack[126], bPack[127] );
        }
#ifdef __AVR__
#else
        Thread::wait(100);
#endif
    }
}


/* -------------------------------------------------------------------- */
// the setup routine runs once when you press reset:
void setup() {
  ///Serial.begin(9600);
  Serial.begin(115200);  
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
    
  ledTimer.every(600, ledBlink2);
  datainTimer.every(300, dataIn_thread);
  datacheckTimer.every(5, dataCheck_thread);
  dataparseTimer.every (100, dataParse_thread);
}

// the loop routine runs over and over again forever:
void loop() {
  unsigned int uiCnt = 0;

  dataparseTimer.update();
  datacheckTimer.update();
  datainTimer.update();
  ledTimer.update();

  ///spDebug("This is a test[%u].\r\n", uiCnt++ );
  ///Serial.print(uiCnt);
  ///Serial.print("\r\n");
  Serial.print(getavaliabe2dataBuffer(&rawDataBuf));
  Serial.print("\r\n");
  uiCnt++;
}
