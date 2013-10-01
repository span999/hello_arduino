/*
  Base on Blink example, we create new function for 
  liquidcrystal & current sensor.

*/

#include <Wire.h>  // Comes with Arduino IDE

#define  CURRENT_SENSOR              0
#define  LIQUID_CRYSTAL_DISPLAY      0

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

#include <Metro.h>




/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */


void _LedCtrl(int state);
void _1blink(int wait);
void _LedOn(int wait);
void _LedOff(int wait);
void _CheckSerialIn(void);
void _LedBlink(void);

#if LIQUID_CRYSTAL_DISPLAY
void _LcdSetup(void);
#endif /* #if LIQUID_CRYSTAL_DISPLAY */

#if CURRENT_SENSOR
int _CurrADC(void);
void _CheckCurrADC(void);
#endif /* #if CURRENT_SENSOR */





#if LIQUID_CRYSTAL_DISPLAY
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
//LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD I2C address
#endif /* #if LIQUID_CRYSTAL_DISPLAY */

Metro ledMetro = Metro(50);
Metro serialMetro = Metro(300);
#if CURRENT_SENSOR
Metro currMetro = Metro(50);
#endif /* #if CURRENT_SENSOR */

int tmp = 0;



// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards.
  pinMode(13, OUTPUT);
   
  ///Serial.begin(9600);  // Used to type in characters
  Serial.begin(57600);  // Used to type in characters

#if LIQUID_CRYSTAL_DISPLAY  
  _LcdSetup();
#endif /* #if LIQUID_CRYSTAL_DISPLAY */
  
}

// the loop routine runs over and over again forever:
void loop() {

  if (serialMetro.check() == 1)
    _CheckSerialIn();

#if CURRENT_SENSOR
  if (currMetro.check() == 1)
    _CheckCurrADC();
#endif /* #if CURRENT_SENSOR */
  
  if (ledMetro.check() == 1)
    _LedBlink();

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
