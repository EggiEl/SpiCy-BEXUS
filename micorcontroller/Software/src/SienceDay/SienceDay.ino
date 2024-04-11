/*Main programm coordinating the different sections*/
#include <Arduino.h>
#include <stdio.h>

#define DEBUG 1
#define MICROS 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)

#if MICROS == 1
#define MESSURETIME_START      \
  unsigned long ta = micros(); \
  unsigned long tb;
#define MESSURETIME_STOP \
  debug("-Time:");       \
  tb = micros() - ta;    \
  debug(tb);             \
  debug("us");
#else
#define MESSURETIME_START      \
  unsigned long ta = millis(); \
  unsigned long tb;
#define MESSURETIME_STOP \
  debug("-Time:");       \
  tb = millis() - ta;    \
  debug(tb);             \
  debug("ms");
#endif

#else
#define debug(x)
#define debugln(x)
#define MESSURETIME_START
#define MESSURETIME_STOP
#endif

#define GREENLEDPIN 10
#define BUBLEPIN 7
//------------------------core1--------------------------------
void setup()
{
  pinMode(GREENLEDPIN, OUTPUT);
  pinMode(BUBLEPIN, OUTPUT);

  analogWrite(GREENLEDPIN, 5);
  analogWrite(BUBLEPIN, 5);

  Serial.begin(115200);
  Serial.setTimeout(1000);

  while (!Serial)
  {
  }
  debugln("\n----------Main is running-C0-------------------\n");
}

 int b = 1023;
void loop()
{
  int a = Serial.parseInt();
 
  if (a)
  {
    b = a;
  }
  delay(10);

  analogWrite(GREENLEDPIN, 450);
  analogWrite(BUBLEPIN, b);
  delay(3000);
  digitalWrite(BUBLEPIN, 0);
  digitalWrite(GREENLEDPIN, 0);
  delay(6000);
}
