
#include <SPI.h>
#include "myCFG.h"


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


#ifdef _USE_SPI_
void sp_doSPItransfer() {
  byte SPIdata[12] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c };
  byte Cnt = 0;

  Serial.println("start SPI transfer.....");
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

void mySPI_setup() {
  // put your setup code here, to run once:
#ifndef _USE_SPI_
#endif

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

void mySPI_loop() {
  // put your main code here, to run repeatedly:

#ifdef _USE_SPI_
  ///sp_doSPItransfer();
#endif
}
