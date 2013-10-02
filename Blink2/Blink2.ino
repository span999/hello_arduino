/*
  Base on Blink example, we create new function for 
 liquidcrystal & current sensor.
 LCD refer:
 http://arduino-info.wikispaces.com/LCD-Blue-I2C
 RF24 refer:
 http://playground.arduino.cc/InterfacingWithHardware/Nrf24L01
 http://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
 
 */

#include <Wire.h>  // Comes with Arduino IDE

#define  LED_BLINK                   0
#define  CURRENT_SENSOR              0
#define  LIQUID_CRYSTAL_DISPLAY      0
#define  SERIAL1_MONITOR             0
#define  RF24_TX                     0
#define  RF24_RX                     1


#if LIQUID_CRYSTAL_DISPLAY
#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR3W.h>
#endif /* #if LIQUID_CRYSTAL_DISPLAY */

#if (RF24_TX || RF24_RX)
#include <SPI.h> /* Comes with Arduino IDE */
#include <RF24.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include "printf.h"
#endif /* #if (RF24_TX || RF24_RX) */

#include <Metro.h>




/*
  Blink
 Turns on an LED on for one second, then off for one second, repeatedly.
 
 This example code is in the public domain.
 */

#if LED_BLINK
void _LedCtrl(int state);
void _1blink(int wait);
void _LedOn(int wait);
void _LedOff(int wait);
void _LedBlink(void);
#endif /* #if LED_BLINK */

void _CheckSerialIn(void);

#if LIQUID_CRYSTAL_DISPLAY
void _LcdSetup(void);
#endif /* #if LIQUID_CRYSTAL_DISPLAY */

#if CURRENT_SENSOR
int _CurrADC(void);
void _CheckCurrADC(void);
#endif /* #if CURRENT_SENSOR */

#if SERIAL1_MONITOR
void _CheckSerial1In(void);
#endif /* #if SERIAL1_MONITOR */

#if RF24_TX
void _Rf24TxSetup(void);
#define  CEPIN    48
#define  CSPIN    49
/* RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps */
#define  RF24_SPEED    RF24_1MBPS
/* RF24_PA_MIN(0)=-18dBm, RF24_PA_LOW(1)=-12dBm, RF24_PA_HIGH(2)=-6dBM, and RF24_PA_MAX(3)=0dBm. */
#define  RF24_POWER    RF24_PA_HIGH
#define  RF24_PAYLOAD  8
void _CheckRf24Tx(void);
#endif /* #if RF24_TX */

#if RF24_RX
void _Rf24RxSetup(void);
#define  CEPIN    8
#define  CSPIN    9
/* RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps */
#define  RF24_SPEED    RF24_1MBPS
/* RF24_PA_MIN(0)=-18dBm, RF24_PA_LOW(1)=-12dBm, RF24_PA_HIGH(2)=-6dBM, and RF24_PA_MAX(3)=0dBm. */
#define  RF24_POWER    RF24_PA_HIGH
#define  RF24_PAYLOAD  8
void _CheckRf24Rx(void);
#endif /* #if RF24_RX */


#if LIQUID_CRYSTAL_DISPLAY
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
//LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD I2C address
#endif /* #if LIQUID_CRYSTAL_DISPLAY */

#if LED_BLINK
Metro ledMetro = Metro(50);
#endif /* #if LED_BLINK */
Metro serialMetro = Metro(300);
#if CURRENT_SENSOR
Metro currMetro = Metro(50);
#endif /* #if CURRENT_SENSOR */
#if SERIAL1_MONITOR
Metro serial1Metro = Metro(200);
#endif /* #if SERIAL1_MONITOR */

#if (RF24_TX || RF24_RX)
// Set up nRF24L01 radio on SPI bus plus CEpin & CSpin
RF24 radio(CEPIN,CSPIN);
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
///const uint64_t pipes[2] = { 0xF0F0F0F0F0LL, 0xF0F0F0F0AALL };
#endif /* #if (RF24_TX || RF24_RX) */

#if RF24_TX
Metro Rf24TxMetro = Metro(1000);
#endif /* #if RF24_TX */
#if RF24_RX
Metro Rf24RxMetro = Metro(50);
#endif /* #if RF24_RX */


int tmp = 0;



// the setup routine runs once when you press reset:
void setup() {
#if LED_BLINK
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards.
  pinMode(13, OUTPUT);
#endif /* #if LED_BLINK */

  ///Serial.begin(9600);  // Used to type in characters
  Serial.begin(57600);  // Used to type in characters

#if SERIAL1_MONITOR
  Serial1.begin(115200);  // Used to type in characters
#endif /* #if SERIAL1_MONITOR */

#if LIQUID_CRYSTAL_DISPLAY  
  _LcdSetup();
#endif /* #if LIQUID_CRYSTAL_DISPLAY */

#if RF24_TX
  _Rf24TxSetup();
#endif /* #if RF24_TX */

#if RF24_RX
  _Rf24RxSetup();
#endif /* #if RF24_RX */

}

// the loop routine runs over and over again forever:
void loop() {

  if (serialMetro.check() == 1)
    _CheckSerialIn();

#if CURRENT_SENSOR
  if (currMetro.check() == 1)
    _CheckCurrADC();
#endif /* #if CURRENT_SENSOR */

#if SERIAL1_MONITOR
  if (serial1Metro.check() == 1)
    _CheckSerial1In();
#endif /* #if SERIAL1_MONITOR */

#if RF24_TX
  if (Rf24TxMetro.check() == 1)
    _CheckRf24Tx();
#endif /* #if RF24_TX */

#if RF24_RX
  if (Rf24RxMetro.check() == 1)
    _CheckRf24Rx();
#endif /* #if RF24_TX */

#if LED_BLINK
  if (ledMetro.check() == 1)
    _LedBlink();
#endif /* #if LED_BLINK */

}


#if LIQUID_CRYSTAL_DISPLAY
/* setup for lcd panel */
void _LcdSetup(void) {

  lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight

  // ------- Quick 3 blinks of backlight  -------------
  for(tmp = 0; tmp< 3; tmp++)
  {
    lcd.backlight();
    delay(150);
    lcd.noBacklight();
    delay(150);
  }
  lcd.backlight(); // finish with backlight on  

  //-------- Write characters on the display ------------------
  // NOTE: Cursor Position: (CHAR, LINE) start at 0  
  lcd.setCursor(0,0); //Start at character 4 on line 0
  lcd.print("Hello, world!");
  delay(500);
  lcd.setCursor(0,1);
  lcd.print("HI! My Arduino*");
  delay(1000);

  // Wait and then tell user they can start the Serial Monitor and type in characters to
  // Display. (Set Serial Monitor option to "No Line Ending")
  lcd.clear();
  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd.print("Use Serial IN");
  lcd.setCursor(0,1);
  lcd.print("Type to display");  
}
#endif /* #if LIQUID_CRYSTAL_DISPLAY */


#if LED_BLINK
/* thread for blink */
void _LedBlink(void) {
  static long lCnt = 0;
  static long lNext = 0;
  static int ledState = LOW;

  if (lCnt == lNext)
  {
    if (ledState == LOW)
    {
      ledState = HIGH;
      lNext = lCnt + 2;  // wait 2 count later
    }
    else
    {
      ledState = LOW;
      lNext = lCnt + 30;  // wait 10 count later
    }
    _LedCtrl(ledState);
  }

  lCnt++;
}
#endif /* #if LED_BLINK */

/* thread for serial in */
void _CheckSerialIn(void) {
  char gChar = 0;
  char preChar = 0;

  // when characters arrive over the serial port...
  if (Serial.available())
  {
    // wait a bit for the entire message to arrive
    delay(100);

#if LIQUID_CRYSTAL_DISPLAY
    // clear the screen
    lcd.clear();
#endif /* #if LIQUID_CRYSTAL_DISPLAY */

    // read all the available characters
    while (Serial.available() > 0)
    {
      // get char from serial
      gChar = Serial.read();
      // check key combination
      if( ('$' == preChar) && ('#' == gChar) )
      {
#if LIQUID_CRYSTAL_DISPLAY
        lcd.print("escape code:");
#endif /* #if LIQUID_CRYSTAL_DISPLAY */
        Serial.print("escape code:");
      }
#if LIQUID_CRYSTAL_DISPLAY
      // display each character to the LCD
      lcd.write(gChar);
#endif /* #if LIQUID_CRYSTAL_DISPLAY */

      // also do the echo on serial
      Serial.write(gChar);
      // update the stored char
      preChar = gChar;
    }
  }
} 


#if SERIAL1_MONITOR
/* thread for serial in */
void _CheckSerial1In(void) {
  char gChar = 0;

  // when characters arrive over the serial port...
  if (Serial1.available())
  {
    // wait a bit for the entire message to arrive
    delay(100);

    // read all the available characters
    while (Serial1.available() > 0)
    {
      // get char from serial
      gChar = Serial1.read();
      // also do the echo on serial 
      Serial.write(gChar);

    }
  }
} 
#endif /* #if SERIAL1_MONITOR */


#if RF24_TX
static bool _IsRf24TxChipReady(void)
{
  bool Rf24TxOn = 0;

  Rf24TxOn = radio.isPVariant();
  if(!Rf24TxOn)
    printf("RF24 nRF24L01+ check %s!!\r\n", (Rf24TxOn)?"ok":"fail");

  return Rf24TxOn;
}

static bool _Rf24TxSetPALevel(rf24_pa_dbm_e Level)
{
  /* RF24_PA_MIN(0)=-18dBm, RF24_PA_LOW(1)=-12dBm, RF24_PA_HIGH(2)=-6dBM, and RF24_PA_MAX(3)=0dBm. */
  bool bRet = false;
  rf24_pa_dbm_e getLev;

  radio.setPALevel(Level);
  getLev = radio.getPALevel();

  if(getLev == Level)
    bRet = true;
  else
    printf("radio.setPALevel=%d, but getPALevel=%d\r\n", Level, getLev);    

  return bRet;
}

void _Rf24TxSetup(void)
{
  printf_begin();
  printf("\n\rRF24/Tx/\n\r");
  //
  // Setup and configure rf radio
  //
  radio.begin();
  printf("radio.begin set...\r\n");

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  printf("radio.setRetries set...\r\n");

  // Become the primary transmitter (ping out)
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  printf("radio.Pipe set...\r\n");

  _IsRf24TxChipReady();
  _Rf24TxSetPALevel(RF24_POWER);  
  radio.setDataRate(RF24_SPEED);
  // optionally, reduce the payload size.  seems to improve reliability
  radio.setPayloadSize(RF24_PAYLOAD);
  printf("radio.getPayloadSize=%d\r\n", radio.getPayloadSize());
  printf("radio.getDataRate=%d\r\n", radio.getDataRate());
  radio.disableCRC();

  radio.printDetails();
  if (!_IsRf24TxChipReady())
    return;

  radio.printDetails();
}

void _CheckRf24Tx(void)
{
  unsigned long time = 0;
  bool bOk = false;

  if (!_IsRf24TxChipReady())
  {
    printf("RF24 Tx setup fail!! retry it ... ");
    _Rf24TxSetup();
    return;
  }
  // Take the time, and send it.  This will block until complete
  time = millis();

  // First, stop listening so we can talk.
  //radio.stopListening();

  bOk = radio.write( &time, sizeof(unsigned long) );
  printf("Now sending %lu(%lu) => %s\r\n", time, millis()-time, (bOk)?"ok":"fail");
  if(!bOk)
  {
    printf("Carrier already? => %s, Signal already? => %s\r\n", (radio.testCarrier())?"yes":"no", (radio.testRPD())?"yes":"no");
    //printf("Signal already? => %s\r\n", (radio.testRPD())?"yes":"no");
  }

}
#endif /* #if RF24_TX */

#if RF24_RX
static bool _IsRf24RxChipReady(void)
{
  bool Rf24RxOn = 0;

  Rf24RxOn = radio.isPVariant();
  if(!Rf24RxOn)
    printf("RF24 nRF24L01+ check %s!!\r\n", (Rf24RxOn)?"ok":"fail");

  return Rf24RxOn;
}

static bool _Rf24RxSetPALevel(rf24_pa_dbm_e Level)
{
  /* RF24_PA_MIN(0)=-18dBm, RF24_PA_LOW(1)=-12dBm, RF24_PA_HIGH(2)=-6dBM, and RF24_PA_MAX(3)=0dBm. */
  bool bRet = false;
  rf24_pa_dbm_e getLev;

  radio.setPALevel(Level);
  getLev = radio.getPALevel();

  if(getLev == Level)
    bRet = true;
  else
    printf("radio.setPALevel=%d, but getPALevel=%d\r\n", Level, getLev);    

  return bRet;
}

static unsigned long started_waiting_at = 0;
void _Rf24RxSetup(void)
{
  printf_begin();
  printf("\n\rRF24/Rx/\n\r");
  //
  // Setup and configure rf radio
  //
  radio.begin();
  printf("radio.begin set...\r\n");

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  printf("radio.setRetries set...\r\n");

  // Become the primary receiver (pong back)
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  printf("radio.Pipe set...\r\n");

  _IsRf24RxChipReady();
  _Rf24RxSetPALevel(RF24_POWER);  
  radio.setDataRate(RF24_SPEED);
  // optionally, reduce the payload size.  seems to improve reliability
  radio.setPayloadSize(RF24_PAYLOAD);
  printf("radio.getPayloadSize=%d\r\n", radio.getPayloadSize());
  printf("radio.getDataRate=%d\r\n", radio.getDataRate());
  radio.disableCRC();

  radio.printDetails();
  if (!_IsRf24RxChipReady())
    return;

  printf("radio.startListening starts...\r\n");
  started_waiting_at = millis();
  // Now, continue listening
  radio.startListening();
}

void _CheckRf24Rx(void)
{
  // Wait here until we get a response, or timeout (250ms)
  ///unsigned long started_waiting_at = millis();
  bool timeout = false;

  if (!_IsRf24RxChipReady())
  {
    printf("RF24 Rx setup fail!! retry it ... \r\n");
    _Rf24RxSetup();
    return;
  }
  ///radio.printDetails();

#if 0
  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 200 )
      timeout = true;
#else  
  if (!radio.available() )
  {
    ///printf(".");
    return;
  }
#endif

  // Describe the results
  if ( timeout )
  {
    printf("Failed, response timed out.\n\r");
  }
  else
  {
    // Grab the response, compare, and send to debugging spew
    unsigned long got_time;
    bool done = false;

    done = radio.read( &got_time, sizeof(unsigned long) );
    if(!done)
      printf("radio.read=%s\r\n", (done)?"ok!":"fail!");
    // Spew it
    //printf("\r\nGot response %lu, round-trip delay: %lu\r\n", got_time, millis()-got_time);
    printf("\r\nGot response %lu, round-trip delay: %lu\r\n", got_time, millis()-started_waiting_at);
  }

  started_waiting_at = millis();
}
#endif /* #if RF24_RX */



#if CURRENT_SENSOR
/* thread for current sensor check */
void _CheckCurrADC(void) {
  static int iCnt = 0;
  int NewADC = 0;
  const int baseADC = 513;
  const int sampleNum = 10;
  static long AccADC = 0;


  NewADC = _CurrADC();
  NewADC = NewADC - baseADC;
  AccADC = AccADC + (NewADC*NewADC);
  iCnt++;  

  if (iCnt > sampleNum)
  {
    float rms = 0;
    float rawrms = 0;

    rawrms = sqrt((float)AccADC/(float)sampleNum);
    rawrms = (rawrms * 75.7576) / 1024.0;
    if ( rawrms < 0.14 )
      rms = (float)0.0;
    else
      rms = 342.857 * rawrms;

#if LIQUID_CRYSTAL_DISPLAY
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(AccADC);
    lcd.print(",");
    lcd.print(rawrms);
    lcd.setCursor(0,1);
    lcd.print("Icurr=");
    lcd.print(rms);
    lcd.print("mA");
#endif /* #if LIQUID_CRYSTAL_DISPLAY */

    Serial.print("\r\n");
    Serial.print(NewADC);
    Serial.print(",");
    Serial.print(rms);
    iCnt = 0;
    AccADC = 0;
  } 
}

/* read ADC from pin */
int _CurrADC(void) {
  return analogRead(3);
}
#endif /* #if CURRENT_SENSOR */

#if LED_BLINK
/* led pin control */
void _LedCtrl(int state) {
  // Pin 13 has an LED connected on most Arduino boards.
  digitalWrite(13, state);
}

/* led on */
void _LedOn(int wait) {
  _LedCtrl(HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(wait);               // wait for a second
}

/* led off */
void _LedOff(int wait) {
  _LedCtrl(LOW);    // turn the LED off by making the voltage LOW
  delay(wait);               // wait for a second
}

/* led blink */
void _1blink(int wait) {
  _LedOn(wait);
  _LedOff(wait);
}
#endif /* #if LED_BLINK */
