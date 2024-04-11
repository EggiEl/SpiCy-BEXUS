/*Main programm coordinating the different sections*/
#include <Arduino.h>
#include <stdio.h>
#include <Ethernet.h>
#include <SD.h>
// #include <SPI.h>  // #include <SoftwareSerial.h>

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

// https://arduino-pico.readthedocs.io/en/latest/index.html

extern volatile uint8_t SD_init;
extern volatile uint8_t TCP_init;

struct packet
{ // struct_format L L 6L 6f 6f 6i i f 2i 80s
  unsigned long id = 0;
  unsigned long timestampPacket = 0;

  unsigned long timestampOxy[6] = {0};

  float oxigen[6] = {0.0f};
  float tempTube[6] = {0.0f};

  int heaterPWM[6] = {0};
  int error = 0;

  float tempCpu = analogReadTemp(3.3f);
  unsigned long power[2] = {0};

  char info[80] = {0};
};

//------------------------core1--------------------------------
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  Serial.setTimeout(1000);

  while (!Serial)
  {
  }
  delay(50);
  debugln("\n<<Main is running-C0>>");
  debugln("/? for help\n");
}

void loop()
{
  serial_commands();
}

//-------------------------core2------------------------
void setup1()
{
  // debugln("----------Main is running-C1-------------------");
}

void loop1()
{
  // if (TCP_init) { fadeLED(); }
  // heartbeat();
}