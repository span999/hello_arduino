/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

#include <stdarg.h>

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;


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



/* -------------------------------------------------------------------- */
// the setup routine runs once when you press reset:
void setup() {
  ///Serial.begin(9600);
  Serial.begin(115200);  
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
}

// the loop routine runs over and over again forever:
void loop() {
  unsigned int uiCnt = 0;
#if 0
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
#else
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(300);           // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(300);               // wait for a second
#endif
  spDebug("This is a test[%u].\r\n", uiCnt++ );
  Serial.print("123\r\n");
  uiCnt++;
}
