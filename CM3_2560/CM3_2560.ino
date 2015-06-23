
// include the SoftwareSerial library so you can use its functions:
#include <SoftwareSerial.h>
#include <Timer.h>
#include "myCFG.h"

#ifdef _USE_SPI_
#include "mySPI.h"
#endif
#ifdef _USE_CM3_
#include "myCM3.h"
#endif
#ifdef _USE_CRC_
#include "myCRC.h"
#endif

#ifdef _USE_SWCOM_
  #ifdef __MEGA2560__
#define rxPin 19
#define txPin 18
  #else
#define rxPin 9
#define txPin 8
  #endif
#endif




Timer ledTimer;
#ifdef _USE_OBD_
Timer odbpatternTimer;
#endif
#ifdef _USE_CM3_
Timer cm3stateTimer;
#endif
#ifdef _USE_SWCOM_
Timer serialRxTimer;
// set up a new serial port
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);
///SoftwareSerial mySerial(rxPin, txPin);
#endif



void sp_printSystemTick() {
  unsigned int mTime = 0;

  /// get system tick in mini-sec
  mTime = millis();
  ///Serial.print("system tick = %l", mTime);
  Serial.print("system tick = ");
  Serial.println(mTime, DEC);
  ///printf("system tick = %d\r\n, mTime");
}

void sp_setLedHi() {
///#ifndef _USE_SPI_
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
///#endif
}

void sp_setLedLo() {
///#ifndef _USE_SPI_
  digitalWrite(13, LOW);   // turn the LED on (HIGH is the voltage level)
///#endif
}

void sp_doLedDelay() {
  delay(200);              // wait for ?? mini-second
  ///delayMicroseconds(200000);  // wait for ?? micro-second
}


#ifdef _USE_OBD_
void sp_doOBDPattern()
{
  byte bPattern1[] = {
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

  byte bChkSum = 0;
  static byte b004 = 0;    ///ECU connetion status
  static short s005 = 0;   ///TPS[5-6] status
  static byte b014 = 0;    ///votage status
  static short s015 = 0;   ///TPS[15-16] status
  static byte b127 = 0;
  unsigned int iLoop = 0;

  b004++;
  s005++;
  b014++;
  s015++;
  b127++;
  bPattern1[3] = (b004 % 2);
  bPattern1[4] = (s005 >> 8) & 0xFF;
  bPattern1[5] = (s005 >> 0) & 0xFF;
  bPattern1[13] = b014;
  bPattern1[14] = (s015 >> 8) & 0xFF;
  bPattern1[15] = (s015 >> 0) & 0xFF;
  bPattern1[126] = b127;

  for ( iLoop = 0; iLoop < 127; iLoop++ ) {
    bChkSum = bChkSum + bPattern1[iLoop];
  }

  ///mySerial.println("456");
  mySerial.write(bPattern1, 127);
  mySerial.write(bChkSum);
  ///mySerial.println("123");

  Serial.write(bPattern1, 127);
  Serial.write(bChkSum);

  ///debug
  Serial.print("\r\n");
  Serial.print("Checksum = 0x");
  ///Serial.println(bChkSum, HEX);
  Serial.print(bChkSum, HEX);
  Serial.print("[");
  ///Serial.println(bChkSum, DEC);
  Serial.print(bChkSum, DEC);
  Serial.print("]");
  Serial.print("\r\n");
}
#endif



void sp_doledUpdate() {
  static boolean bON = false;
  ///static unsigned short usCnt = 0;
  
  if ( bON ) {
    sp_setLedHi();
  } else {
    sp_setLedLo();
  }
  ///sp_doLedDelay();
  sp_printSystemTick();
  bON = !bON;

#ifdef _USE_SPI_
  sp_doSPItransfer();
#endif

  ///doCMstate();

}

#ifdef _USE_CM3_
void sp_docm3stateUpdate() {
  static unsigned int uiCnt = 0;
  
  doCMstate( millis() );
  uiCnt++;
}
#endif

#ifdef _USE_SWCOM_
void sp_doserialRx() {
  unsigned char ucTmp = 0;

  Serial.print("-");
  if( 0 < mySerial.overflow() ) {
    Serial.println(">>>>SoftwareSerial overflow!!!");
  }
  ///while( 0 < mySerial.available() ) {
  if( 0 < mySerial.available() ) {
    ucTmp = mySerial.read();
    Serial.print(">");
    Serial.print(ucTmp, DEC);
    Serial.print(", 0x");
    Serial.print(ucTmp, HEX);
    Serial.print(", [");
    Serial.write(ucTmp);
    Serial.println("]");
    doCM3parsing(ucTmp);
    #if 0
    ///echo to terminal
    if ( 0x0d == ucTmp ) {
      mySerial.println("\t");
    } else {
      mySerial.write(ucTmp);
    }
    #endif
    ///break;
  }
}
#else
void serialEvent1() {
  unsigned char ucTmp = 0;

  if ( 1 ) {
    ucTmp = Serial1.read();
    #if 0
    Serial.print(">>");
    Serial.print(ucTmp, DEC);
    Serial.print(", 0x");
    Serial.print(ucTmp, HEX);
    Serial.print(", [");
    Serial.write(ucTmp);
    Serial.println("]");
    #endif
    doCM3parsing(ucTmp);
    #if 0
    ///echo to terminal
    if ( 0x0d == ucTmp ) {
      mySerial.println("\t");
    } else {
      mySerial.write(ucTmp);
    }
    #endif
  }  
}
#endif



/* -------------------------------------------------------------------------------------- */

void setup() {
  // put your setup code here, to run once:
  // initialize debug serial port
  ///Serial.begin(9600);
  Serial.begin(115200);

  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);

#ifdef _USE_SWCOM_
  ///pinMode(txPin, OUTPUT);
  ///pinMode(rxPin, INPUT);
  ///mySerial.begin(19200);
  ///mySerial.begin(9600);
  mySerial.begin(115200);
  mySerial.listen();
  ///mySerial.println("Hello World - SoftwareSerial");
#else
  Serial1.begin(115200);
  ///Serial1.listen();///use serialEvent1();
  ///Serial1.println("Hello World - Serial1");
#endif
  
  ///ledTimer.every(600, sp_doledUpdate);
  ledTimer.every(1000, sp_doledUpdate);
  ///ledTimer.every(3000, sp_doledUpdate);
  ///ledTimer.every(2000, sp_doledUpdate);
#ifdef _USE_OBD_
  odbpatternTimer.every(300, sp_doOBDPattern);
#endif
#ifdef _USE_CM3_
  cm3stateTimer.every(250, sp_docm3stateUpdate);
#endif
#ifdef _USE_SWCOM_
  serialRxTimer.every(10, sp_doserialRx);
  ///serialRxTimer.every(2, sp_doserialRx);
#endif

#ifdef _USE_SPI_
  mySPI_setup();
#endif
}

void loop() {
  // put your main code here, to run repeatedly:
  ledTimer.update();
#ifdef _USE_OBD_ 
  odbpatternTimer.update();
#endif
#ifdef _USE_CM3_
  cm3stateTimer.update();
#endif
#ifdef _USE_SWCOM_
  serialRxTimer.update();
#endif

#ifdef _USE_SPI_
  mySPI_loop();
#endif
}
