
// include the SoftwareSerial library so you can use its functions:
#include <SoftwareSerial.h>
#include <SPI.h>

#define _USE_SPI_

#define rxPin 9
#define txPin 8

#ifdef _USE_SPI_
// set up the speed, data order and data mode
SPISettings settingsA(2000000, MSBFIRST, SPI_MODE1);
SPISettings settingsB(16000000, LSBFIRST, SPI_MODE3);
// using two incompatible SPI devices, A and B. Incompatible means that they need different SPI_MODE
const int slaveAPin = 10;
const int slaveBPin = 10;
const int chipSelectPin = slaveBPin;
const byte spiREAD = 0b11111100;     // SCP1000's read command
const byte spiWRITE = 0b00000010;   // SCP1000's write command
#endif

// set up a new serial port
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);
///SoftwareSerial mySerial(rxPin, txPin);

void sp_printSystemTick() {
  unsigned int mTime = 0;

  /// get system tick in mini-sec
  mTime = millis();
  ///printf("system tick = %l", mTime);
  ///Serial.print("system tick = %l", mTime);
  Serial.print("system tick = ");
  Serial.println(mTime);
  ///Serial.print("\r\n");
}

void sp_setLedHi() {
#ifndef _USE_SPI_
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
#endif
}

void sp_setLedLo() {
#ifndef _USE_SPI_
  digitalWrite(13, LOW);   // turn the LED on (HIGH is the voltage level)
#endif
}

void sp_doLedDelay() {
  delay(200);              // wait for ?? mini-second
  ///delayMicroseconds(200000);  // wait for ?? micro-second
}

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

#ifdef _USE_SPI_
void sp_doSPItransfer() {
  byte SPIdata[12] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c };
  byte Cnt = 0;

  // send result to SPI slave device
  SPI.beginTransaction(settingsB);
  digitalWrite (slaveBPin, LOW);
  while ( 12 > Cnt++ ) {
    SPI.transfer(SPIdata[Cnt]);
  }
  digitalWrite (slaveBPin, HIGH);
  SPI.endTransaction();
}

//Read from or write to register from the SCP1000:
unsigned int readRegister(byte thisRegister, int bytesToRead ) {
  byte inByte = 0;           // incoming byte from the SPI
  unsigned int result = 0;   // result to return
  Serial.print(thisRegister, BIN);
  Serial.print("\t");
  // SCP1000 expects the register name in the upper 6 bits
  // of the byte. So shift the bits left by two bits:
  thisRegister = thisRegister << 2;
  // now combine the address and the command into one byte
  byte dataToSend = thisRegister & spiREAD;
  Serial.println(thisRegister, BIN);
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);
  // send the device the register you want to read:
  SPI.transfer(dataToSend);
  // send a value of 0 to read the first byte returned:
  result = SPI.transfer(0x00);
  // decrement the number of bytes left to read:
  bytesToRead--;
  // if you still have another byte to read:
  if (bytesToRead > 0) {
    // shift the first byte left, then get the second byte:
    result = result << 8;
    inByte = SPI.transfer(0x00);
    // combine the byte you just got with the previous one:
    result = result | inByte;
    // decrement the number of bytes left to read:
    bytesToRead--;
  }
  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
  // return the result:
  return (result);
}

//Sends a write command to SCP1000
void writeRegister(byte thisRegister, byte thisValue) {
  // SCP1000 expects the register address in the upper 6 bits
  // of the byte. So shift the bits left by two bits:
  thisRegister = thisRegister << 2;
  // now combine the register address and the command into one byte:
  byte dataToSend = thisRegister | spiWRITE;

  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);

  SPI.transfer(dataToSend); //Send register location
  SPI.transfer(thisValue);  //Send value to record into register

  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
}
#endif

/* -------------------------------------------------------------------------------------- */

void setup() {
  // put your setup code here, to run once:
#ifndef _USE_SPI_
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
#endif
  // initialize serial port
  Serial.begin(9600);

  mySerial.begin(19200);
  ///mySerial.begin(9600);
  mySerial.println("Hello World - SoftwareSerial");

#ifdef _USE_SPI_
  // start the SPI library:
  SPI.begin();
  // initalize the  data ready and chip select pins:
  ///pinMode(dataReadyPin, INPUT);
  ///pinMode(chipSelectPin, OUTPUT);
  pinMode(slaveAPin, OUTPUT);
  pinMode(slaveBPin, OUTPUT);
#endif
}

void loop() {
  // put your main code here, to run repeatedly:
  sp_setLedHi();
  sp_doLedDelay();
  sp_printSystemTick();
  sp_setLedLo();
  sp_doLedDelay();
  sp_printSystemTick();
  sp_doOBDPattern();
#ifdef _USE_SPI_
  sp_doSPItransfer();
#endif
}
