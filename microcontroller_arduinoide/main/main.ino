#include <Arduino.h>
#include <stdio.h>
// #include <SPI.h>  // #include <SoftwareSerial.h>
#include <Ethernet.h>
#include <SD.h>

#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define MESSURETIME_START \
  unsigned long ta = millis(); \
  unsigned long tb;
#define MESSURETIME_STOP \
  debug("-Time:"); \
  tb = millis() - ta; \
  debug(tb); \
  debug("ms");
#else
#define debug(x)
#define debugln(x)
#define MESSURETIME_START
#define MESSURETIME_STOP
#endif

// https://arduino-pico.readthedocs.io/en/latest/index.html

extern volatile uint8_t SD_init;

struct packet {  // struct_format L L 6L 6f 6f 6i i f 2i 80s
  unsigned long id = 0;
  unsigned long timestampPacket = 0;

  unsigned long timestampOxy[6] = { 0 };

  float oxigen[6] = { 0.0f };
  float tempTube[6] = { 0.0f };

  int heaterPWM[6] = { 0 };
  int error = 0;

  float tempCpu = analogReadTemp(3.3f);
  int power[2] = { 0 };

  char info[80] = { 0 };
};

//------------------------core1--------------------------------
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Serial.begin(20000000);

  Serial.begin(9600);
  while (!Serial) {}
  debugln("\n----------Main is running-C0-------------------\n");
  struct packet a;
  // Serial.println(max_freq_sd());


  // Serial.println(startuptest(), BIN);

  // testSD();

  // testServer();

  struct packet first = packet_create();
  packet_writeinfo(first, "First Packet");
  send_TCP_packet(first);

  // printpacketardesses();
  // init_SD();

  test_TCP();
}


void loop() {
  // Serial.println(millis());
  // Serial.println(analogReadTemp(3.3f));
}


//-------------------------core2------------------------
void setup1() {
  // debugln("----------Main is running-C1-------------------");
}

void loop1() {
  // blinkLed();
  fadeLED();
  // heartbeat();
}