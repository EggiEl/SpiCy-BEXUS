/*-------------Main programm coordinating the different sections-------------------*/
#include "header.h"

// https://arduino-pico.readthedocs.io/en/latest/index.html

//------------------------core1--------------------------------
void setup()
{

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  Serial.setTimeout(1000);
  rp2040.enableDoubleResetBootloader();
  while (!Serial)
  {
  }
  print_startup_message();
}

void loop()
{
  check_periodic_tasks();
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

void check_periodic_tasks()
{
  rp2040.wdt_reset();
}

void print_startup_message()
{
  delay(50);
  debug("<<[MotherboardV4.ino] is running on Chip ");
  debug(rp2040.getChipID());
  debug("|Core ");
  debug(rp2040.cpuid());
  debug("|Freq ");
  debug(rp2040.f_cpu()/1000000.0);
  if (watchdog_caused_reboot())
  {
    debugln("MHz after a Watchdog Reset>>\n");
  }
  else
  {
    debugln("MHz>>\n");
  }

  debugln("\"/?\" for help\n");
}