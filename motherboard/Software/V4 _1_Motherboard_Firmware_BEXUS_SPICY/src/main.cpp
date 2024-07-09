/*-------------Main programm coordinating the different sections-------------------*/
/*
 .-')       _ (`-.                                                 (`-.
 ( OO ).    ( (OO  )                                              _(OO  )_
(_)---\_)  _.`     \   ,-.-')     .-----.    ,--.   ,--.      ,--(_/   ,. \    .---.
/    _ |  (__...--''   |  |OO)   '  .--./     \  `.'  /       \   \   /(__/   / .  |
\  :` `.   |  /  | |   |  |  \   |  |('-.   .-')     /         \   \ /   /   / /|  |
 '..`''.)  |  |_.' |   |  |(_/  /_) |OO  ) (OO  \   /           \   '   /,  / / |  |_
.-._)   \  |  .___.'  ,|  |_.'  ||  |`-'|   |   /  /\_           \     /__)/  '-'    |
\       /  |  |      (_|  |    (_'  '--'\   `-./  /.__)           \   /    `----|  |-'
 `-----'   `--'        `--'       `-----'     `--'                 `-'          `--'
*/

#include "header.h"
#include <EEPROM.h>

// check memory leaks with: platformio check --skip-packages
// check code size: cloc --by-file-by-lang .
// platformio check --skip-packages-- flags = "--enable=all --inconclusive --force --std=c99 --std=c++11"

//  https://arduino-pico.readthedocs.io/en/latest/index.html
void print_startup_message();
void update_nResets();
void check_periodic_tasks();

//------------------------core1--------------------------------
/*does all the data handeling*/
void setup()
{
  rp2040.wdt_begin(WATCHDOG_TIMEOUT);
  update_nResets();
  print_startup_message();
  debugln(oxy_isconnected(NTC_PROBE_0));
  debugln(oxy_isconnected(NTC_PROBE_0));
  // oxy_setup();
}

void loop()
{
  check_periodic_tasks();
  // temp_log("Temp_2W5_cooled.csv", NTC_PROBE_0, NTC_PROBE_2, 100);

  //  debugln(oxy_isconnected(NTC_PROBE_1));
   delay(500);
     debugln(oxy_isconnected(NTC_PROBE_0));
}

/*all things that should get checkt every loop of CPU0*/
void check_periodic_tasks()
{
  checkSerialInput();
  rp2040.wdt_reset();
  StatusLedBlink(STATLED);
  // tcp_check_command();
  // rp2040.wdt_reset();
  // nextState();
  rp2040.wdt_reset();
}

//-------------------------core2------------------------
/*controlls heating*/
void setup1()
{
}

void loop1()
{
  // if (temp_read_one(NTC_PROBE_0) > 30.0)
  // {
  //   heat_updateone(PIN_H0, 0);
  // }
  // else
  // {
  //   heat_updateone(PIN_H0, 100.0);
  // }
}

unsigned long nMOTHERBOARD_BOOTUPS = 0;
/**
 * this function adds one to the nMOTHERBOARD_BOOTUPS counter in the flash
 * keep tracks of how often the Motherboard did boot up
 * */
void update_nResets()
{
  const uint8_t ADRES_NRESETS = 0;

  EEPROM.begin(256);
  union int_to_byte
  {
    int integer;
    byte bytearray[sizeof(int)];
  } buffer;

  // reads a nMOTHERBOARD_BOOTUPSS from EEPROM
  for (int i = 0; i < sizeof(int); i++)
  {
    buffer.bytearray[i] = EEPROM.read(ADRES_NRESETS + i);
  }

  // new reset -> +1
  buffer.integer += 1;

  nMOTHERBOARD_BOOTUPS = buffer.integer;

  // writes the one increased nMOTHERBOARD_BOOTUPSS back to flash
  for (int i = 0; i < sizeof(int); i++)
  {
    EEPROM.write(ADRES_NRESETS + i, buffer.bytearray[i]);
  }

  EEPROM.commit();
  EEPROM.end();
}

/* Prints a message with facts about the MPU like frequency and weather a watchdog restarted it*/
void print_startup_message()
{
#if DEBUG == 1
  Serial.setTimeout(1000);
  Serial.begin();
  for (unsigned int i = 0; i < 200; i++)
  {
    delay(40);
    if (Serial)
    {
      break;
    }
  }

  // ghost
  debugf_green(
      "  .-')       _ (`-.                                       \n"
      " ( OO ).    ( (OO  )                                      \n"
      "(_)---\\_)  _.`     \\   ,-.-')     .-----.    ,--.   ,--.\n"
      "/    _ |  (__...--''   |  |OO)   '  .--./     \\  `.'  /  \n"
      "\\  :` `.   |  /  | |   |  |  \\   |  |('-.   .-')     /  \n"
      " '..`''.)  |  |_.' |   |  |(_/  /_) |OO  ) (OO  \\   /    \n"
      ".-._)   \\  |  .___.'  ,|  |_.'  ||  |`-'|   |   /  /\\_  \n"
      "\\       /  |  |      (_|  |    (_'  '--'\\   `-./  /.__) \n"
      " `-----'   `--'        `--'       `-----'     `--'        \n");
  // graphiti
  // SET_COLOUR_GREEN
  // Serial.println(" _________        .__ _________                   _____  ");
  // Serial.println("/   _____/______  |__|\\_   ___ \\ ___.__. ___  __ /  |  | ");
  // Serial.println("\\_____  \\ \\____ \\ |  |/    \\  \\/<   |  | \\  \\/ //   |  |");
  // Serial.println("/        \\|  |_> >|  |\\     \\____\\___  |  \\   //    ^   /");
  // Serial.println("/_______  /|   __/ |__| \\______  // ____|   \\_/ \\____   | ");
  // Serial.println("        \\/ |__|                \\/ \\/                 |__|");
  // SET_COLOUR_RESET

  if (watchdog_caused_reboot())
  {
    debugf_status(">[MotherboardV4.ino] is running on Chip %i Core %i |Freq %.1f MHz|nResets %u |Watchdog Reset<<\n", rp2040.getChipID(), rp2040.cpuid(), rp2040.f_cpu() / 1000000.0, nMOTHERBOARD_BOOTUPS);
  }
  else
  {
    debugf_status(">[MotherboardV4.ino] is running on Chip %i Core %i |Freq %.1f MHz|nResets %u <<\n", rp2040.getChipID(), rp2040.cpuid(), rp2040.f_cpu() / 1000000.0, nMOTHERBOARD_BOOTUPS);
  }

  debugf_info("\"/?\" for help\n");
  rp2040.enableDoubleResetBootloader();
#endif
}
