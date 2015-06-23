
#include "myCFG.h"

///#define  _DBG_CMD
///#define  _DBG_STAT
///#define  _DBG_PKG
///#define  _DBG_REPLY


#ifdef _USE_CM3_
#define  RESP_ERR_OK       0x50
#define  RESP_ERR_HEAD     0x51
#define  RESP_ERR_CRC      0x52
#define  RESP_ERR_0LEN     0x53
#define  RESP_ERR_BLEN     0x54
#define  RESP_ERR_UNKW     0x55

#define  CMD_ID_STATE      0x50
#define  CMD_ID_CRED       0x43
#define  CMD_ID_ACCESS     0x41
#define  CMD_ID_ENABLE     0x45
#define  CMD_ID_SET        0x53
#define  CMD_ID_GET        0x47
#define  CMD_ID_DIAG       0x44
#define  CMD_ID_UPDATE     0x55
#define  CMD_ID_CONF       0x56

#define  FNI_ID_PROFILE    0x04
#define  FNI_ID_AGE        0x03
#define  FNI_ID_HEIGHT     0x05
#define  FNI_ID_WEIGHT     0x06
#define  FNO_ID_AVG_HR     0x20
#define  FNO_ID_HEARTBEAD  0x27
#define  FNO_ID_SKIN_PROX  0x22
#define  FNO_ID_ACT_ENERGY 0x23
#define  FNO_ID_ACT_TYPE   0x26
#define  FNO_ID_SPEED      0x24
#define  FNO_ID_CADENC     0x25

enum eCM3states{
  statUNKNOW = 0,
  statINIT,
  statVERSION,
  statSN,
  statSN2,
  statCREDEN,
  statACCESS,
  statENABLE,
  statENABLECHK,
  statACTIVE,
  statCHECKSTAT,
};

struct sCM3CmdData {
  unsigned short usLen;
  unsigned char ucData[18];
};

static const struct sCM3CmdData CM3table[] = {
  {  2, { CMD_ID_CONF, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
  {  2, { CMD_ID_CONF, 0x02, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
  {  2, { CMD_ID_CONF, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
  {  1, { CMD_ID_STATE, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
  {  4, { CMD_ID_GET, 0x02, FNO_ID_AVG_HR, FNO_ID_SKIN_PROX, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
  {  3, { CMD_ID_GET, 0x01, FNO_ID_AVG_HR, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
///  { 10, { CMD_ID_CRED, 0x00, 0x07, 0x00, 0x02, 0x04, 0x20, 0x24, 0x25, 0x2B, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
  { 10, { CMD_ID_CRED, 0x02, 0x00, 0x07, 0x00, 0x04, 0x20, 0x24, 0x25, 0x2B, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
  {  5, { CMD_ID_ACCESS, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x24, 0x25, 0x2B, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
  {  2, { CMD_ID_GET, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
///  {  7, { CMD_ID_ENABLE, 0x01, 0x04, FNO_ID_AVG_HR, FNO_ID_SPEED, FNO_ID_CADENC, FNO_ID_HEARTBEAD, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
  {  4, { CMD_ID_ENABLE, 0x01, 0x01, FNO_ID_AVG_HR, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
  {  2, { CMD_ID_ENABLE, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
  {  2, { CMD_ID_STATE, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff} },
  { 14, { CMD_ID_GET, 0x02, 0x20, 0x03, 0x00, 0x01, 0x03, 0x5a, 0x22, 0x03, 0x00, 0x06, 0x04, 0x00, 0xff, 0xff, 0xff, 0xff} },  ///this is reply
  { 18, { CMD_ID_CONF, 0x01, 0x04, 0x00, 0x00, 0x32, 0x30, 0x31, 0x35, 0x30, 0x32, 0x30, 0x35, 0x5f, 0x31, 0x32, 0x32, 0x34} },  ///this is reply
};

unsigned int authorize = 0;
unsigned long AccessKey = 0x13970AC4;
///unsigned long AccessKey = 0xC40A9713;
unsigned char Challenge[16];
unsigned long ACResponse = 0;
eCM3states CM3stat = statINIT;
#endif


#ifdef _USE_CM3_
void doCmd( int iCID ) {
  unsigned char cmdpack[] = { 0x00, 0x81, 0x12, 0x00, 0x56, 0x01, 0x04, 0x00, 0x00, 0x32, 0x30, 0x31, 0x35, 0x30, 0x32, 0x30, 0x35, 0x5f, 0x31, 0x32, 0x32, 0x34, 0x1b, 0xe0 };
  unsigned char *pC = 0;
  unsigned int iCnt = 0;
  unsigned int iTmp = 0;
  ///unsigned short *pusCrc = 0;

  iCID = iCID-1;
  *(unsigned short *)(cmdpack+2) = CM3table[iCID].usLen;
  iCnt = CM3table[iCID].usLen;
  ///load command data
  memcpy( cmdpack+4, CM3table[iCID].ucData, iCnt);

  ///handle Challenge response
  if( CMD_ID_ACCESS == cmdpack[4] ) {
    *(unsigned long*)(cmdpack+5) = ACResponse;
    if ( 0 == ACResponse ) {
      CM3stat = statCHECKSTAT;
    }
  }
  
  ///CRC
  ///pC = cmdpack+4;
  ///*(unsigned short *)(cmdpack+iCnt+4) = gen_crc_ccitt(0xffff, pC, iCnt);
  *(unsigned short *)(cmdpack+iCnt+4) = gen_crc_ccitt(0xffff, cmdpack+4, iCnt);

  ///send to serial
  iCnt = iCnt + 6;
#ifdef _DBG_CMD
  Serial.print("<<<");
#endif
  for ( iTmp = 0; iTmp < iCnt; iTmp++ ) {
    #ifdef _USE_SWCOM_
    mySerial.write(cmdpack[iTmp]);
    #else
    Serial1.write(cmdpack[iTmp]);
    #endif
#ifdef _DBG_CMD
    Serial.print(" 0x");
    Serial.print(cmdpack[iTmp], HEX);
#endif
  }
#ifdef _DBG_CMD
  Serial.println("\t");
#endif
}

static void CMstateOperation( unsigned int systick ) {
  static unsigned int uiCnt = 0;
 
  if ( statINIT == CM3stat ) {
    doCmd(4);
    CM3stat = statVERSION;
  } else
  if ( statVERSION == CM3stat ) {
    doCmd(1);
    CM3stat = statSN;
  } else
  if ( statSN == CM3stat ) {
    doCmd(3);
    CM3stat = statSN2;
  } else
  if ( statSN2 == CM3stat ) {
    doCmd(4);
    CM3stat = statCREDEN;
  } else
  if ( statCHECKSTAT == CM3stat ) {
    doCmd(4);
    ///CM3stat = statCREDEN;
  } else
  if ( statCREDEN == CM3stat ) {
    doCmd(7);
    CM3stat = statACCESS;
  } else
  if ( statACCESS == CM3stat ) {
    doCmd(8);
    ///CM3stat = statACCESS;
  } else
  if ( statENABLE == CM3stat ) {
    doCmd(10);
    CM3stat = statENABLECHK;
  } else
  if ( statENABLECHK == CM3stat ) {
    doCmd(11);
    CM3stat = statACTIVE;
  } else
  if ( statACTIVE == CM3stat ) {
    ///doCmd(6);
    ///switch ( ++uiCnt%5 ) {
    switch ( ++uiCnt%4 ) {
      case 0:
        ///doCmd(10);
        ///break;
      case 1:
        ///doCmd(11);
        ///break;
      case 2:
        doCmd(9);  ///get all
        break;
      case 3:
        doCmd(6);  ///get AHR
        break;
      case 4:
        doCmd(4);  ///get state
        ///delay(2000);
        delay(10);
        doCmd(6);  ///get AHR
        break;
      case 5:
        ///doCmd(10);
        break;
      default:
        doCmd(12);
        break;
    }
  } else
  if ( statUNKNOW == CM3stat ) {
  } else {
  }  
}

void doCMstate( unsigned int systick ) {
  static unsigned int uiCnt = 0;

  if (0 == uiCnt%4 ) {
    //1 sec
    CMstateOperation( systick );
  } else
  if (0 == uiCnt%22 ) {
    doCmd(4);  ///get state
  }
  
  uiCnt++;

  if ( 240 < uiCnt ) {
    uiCnt = 0;
    Serial.println("power down CM3 ...............");
    doCmd(12);
  } else {
#ifdef _DBG_STAT
    Serial.print("state=");
    Serial.print(CM3stat, DEC);
    Serial.print(", count=");
    Serial.print(uiCnt, DEC);
    Serial.print(", tick=");
    Serial.println(systick, DEC);
#endif
  }

}

void doCM3parsing( unsigned char ucByteIn ) {
  static unsigned char CM3reply[32];
  static unsigned int uiCnt = 0;
  static unsigned short usSize = 0;
  static unsigned short usChksum = 0;
  
  CM3reply[uiCnt] = ucByteIn;
  uiCnt++;
  
  if ( 1 == uiCnt ) {
    if( 0x50 == CM3reply[0] ) {
    } else {
      Serial.print("Msg>>wrong resp:0x");
      Serial.print(CM3reply[0], HEX);
      Serial.println("\t");
      uiCnt = 0;
    }
    return;
  }
  if ( 3 == uiCnt ) {
    if ( 0x50 == CM3reply[0] && 0x00 == CM3reply[1] && 0x81 == CM3reply[2]) {
#ifdef _DBG_PKG
      Serial.println("Msg>>pack head!");
#endif
      usSize = 0;
      usChksum = 0;
    } else {
      uiCnt = 0;
      usSize = 0;
      usChksum = 0;
    }
    return;
  } 
  if ( 5 == uiCnt ) {
    ///usSize = (unsigned short)(CM3reply+3);
    usSize = CM3reply[4];
    usSize = usSize << 8;
    usSize = usSize | CM3reply[3];
#ifdef _DBG_PKG
    Serial.print("Msg>>size:");
    Serial.print(usSize, DEC);
    Serial.println("\t");
#endif
  }
  if ( 0 < usSize ) {
    if ( uiCnt == usSize + 7 ) {
      unsigned short usTmp = 0; 
      ///usChksum = (unsigned short)(CM3reply+usSize+5);
      usChksum = CM3reply[usSize+6];
      usChksum = usChksum << 8;
      usChksum = usChksum | CM3reply[usSize+5];
      usTmp = gen_crc_ccitt(0xffff, CM3reply+5, usSize);
#ifdef _DBG_PKG
      Serial.print("Msg>>Len:");
      Serial.print(usSize, DEC);
      Serial.print(",Chksum=0x");
      Serial.print(usChksum, HEX);
      Serial.print(",Chksum=0x");
      Serial.print(usTmp, HEX);
      Serial.println("\t");
#endif
      if ( usChksum == usTmp ) {
        if ( 0x3f == CM3reply[5] ) {
          Serial.println("Msg>>cmd fail, reply error code.");
        } else {
          /// any reply 
          if ( CMD_ID_CRED == CM3reply[5] ) {
            Serial.print("Msg>>get challenge=");
            for (usTmp = 0; usTmp < 16; usTmp++) {
              Challenge[usTmp] = CM3reply[usTmp+6];
              Serial.print(" 0x");
              Serial.print(Challenge[usTmp], HEX);
            }
            Serial.println("\t");
            doCalculateChallengeResponse((unsigned char*)&AccessKey, Challenge, &ACResponse);
            Serial.print("Msg>>get response=0x");
            Serial.print(ACResponse, HEX);
            Serial.println("\t");
          } else 
          if ( CMD_ID_STATE == CM3reply[5] ) {
            Serial.print("Msg>>get CM3 state =0x");
            Serial.print(CM3reply[6], HEX);
            Serial.println("\t");
            if ( 2 == CM3reply[6] ) {  ///active
              CM3stat = statACTIVE;
            } else
            if ( 1 == CM3reply[6] ) {  ///authorizing
              CM3stat = statCREDEN;
            }
          } else 
          if ( CMD_ID_ACCESS == CM3reply[5] ) {
            if ( 0 == CM3reply[6] ) {
              Serial.println("Msg>>get CM3 access rejected!!");
              ///CM3stat = statINIT;
              CM3stat = statCHECKSTAT;
            }
            if ( 1 == CM3reply[6] ) {
              Serial.println("Msg>>get CM3 access granted!!");
              ///CM3stat = statACTIVE;
              CM3stat = statENABLE;
            }
          } else 
          if ( CMD_ID_GET == CM3reply[5] ) {
            if ( FNO_ID_AVG_HR == CM3reply[7] ) {
              Serial.print("Msg>>get AHR >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> I=");
              Serial.print(CM3reply[10],DEC);
              Serial.print(", Q=");
              Serial.print(CM3reply[11],DEC);
              Serial.print(", V=");
              Serial.print(CM3reply[12],DEC);
              Serial.println("\t");
            }
            if ( FNO_ID_SPEED == CM3reply[7] ) {
              Serial.println("Msg>>get SPEED !!");
            }
          }
        }
#ifdef _DBG_PKG
        Serial.print("Msg>>");
        for (usTmp = 0; usTmp < usSize; usTmp++) {
          Serial.print(" ");
          Serial.write(CM3reply[usTmp+5]);
        }
        Serial.println("\t");
#endif
#ifdef _DBG_PKG
        Serial.print("Msg>>");
        for (usTmp = 0; usTmp < usSize; usTmp++) {
          Serial.print(" 0x");
          Serial.print(CM3reply[usTmp+5], HEX);
        }
        Serial.println("\t");
#endif
        /// dump buffer
#ifdef _DBG_REPLY
        Serial.print("Reply>>");
        for (usTmp = 0; usTmp < usSize+5; usTmp++) {
          Serial.print(" 0x");
          Serial.print(CM3reply[usTmp], HEX);
        }
        Serial.println("\t");
#endif
      } else {
        Serial.print("Msg>>checkSum Error!!");
      }
      uiCnt = 0;
      usSize = 0;
      usChksum = 0;
    } else {
      ///Serial.print("Msg>>current ID:");
      ///Serial.print(uiCnt, DEC);
      ///Serial.print("\t");
    }
  }

}

static unsigned long CalculateCrc32(unsigned long initValue, unsigned long polynomial, unsigned char* message, unsigned char msgsize)
{
  unsigned long remainder = 0;
  unsigned char singleByte = 0;
  unsigned char singleBit = 0;
  const unsigned long WIDTH = 32;
  const unsigned long TOPBIT = 0x80000000;

  // Initialize the remainder
  remainder = initValue;
  // Perform modulo-2 division, a byte at a time.
  for ( singleByte = 0; singleByte < msgsize; ++singleByte) {
    // Bring the next byte into the remainder.
    remainder ^= ((unsigned long)message[singleByte] << (long)(WIDTH - 8));

    // Perform modulo-2 division, a bit at a time.
    for (singleBit = 8; singleBit > 0; --singleBit) {
      // Try to divide the current data bit.
      if ((remainder & TOPBIT) == TOPBIT) {
        remainder = (remainder << 1) ^ polynomial;
      } else {
        remainder = (remainder << 1);
      }
    }
  }
  // The final remainder is the CRC result.
  return remainder;
}

void doCalculateChallengeResponse(unsigned char* accessKey, unsigned char* challenge, unsigned long *response)
{
  // Calculate the response based on the accessKey and the latest challengeCode
  const unsigned long Polynomial = 0x04C11DB7;
  const unsigned long InitialCrc = 0x00000000;
  // Hash the challenge
  unsigned long tempCrc32 = 0;
  tempCrc32 = CalculateCrc32(InitialCrc, Polynomial, challenge, 16);
  // Hash the accessKey
  *response = CalculateCrc32(tempCrc32, Polynomial, accessKey, 4);
}

#endif



/* -------------------------------------------------------------------------------------- */

void myCM3_setup() {
  // put your setup code here, to run once:

}

void myCM3_loop() {
  // put your main code here, to run repeatedly:

}
