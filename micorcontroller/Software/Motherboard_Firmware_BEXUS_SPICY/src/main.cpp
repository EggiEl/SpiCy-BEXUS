/*-------------Main programm coordinating the different sections-------------------*/
#include "header.h"

// https://arduino-pico.readthedocs.io/en/latest/index.html
void print_startup_message();
void check_periodic_tasks();

//------------------------core1--------------------------------
void setup()
{
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
  #if DEBUG ==1
  delay(60*60*1000);
  debugln("<<rebooted to bootloader after on hour>>");
  rp2040.rebootToBootloader();
  #endif
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
  #if DEBUG == 1
  // Serial.begin(5000000);
   Serial.begin(115200);
  Serial.setTimeout(1000);
  while (!Serial)
  {
  }
  delay(50);
  debug("\n<<[MotherboardV4.ino] is running on Chip ");
  debug(rp2040.getChipID());
  debug(">>\nCore ");
  debug(rp2040.cpuid());
  debug("|Freq ");
  debug(rp2040.f_cpu() / 1000000.0);
  if (watchdog_caused_reboot())
  {
    debugln("MHz|Watchdog Reset");
  }
  else
  {
    debugln("MHz");
  }
  debugln("\"/?\" for help\n");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  rp2040.enableDoubleResetBootloader();
  #endif
}