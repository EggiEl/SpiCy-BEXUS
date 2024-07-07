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

// check memory leaks with: platformio check --skip-packages
// check code size: cloc --by-file-by-lang .
// platformio check --skip-packages-- flags = "--enable=all --inconclusive --force --std=c99 --std=c++11"

//  https://arduino-pico.readthedocs.io/en/latest/index.html
void print_startup_message();
void check_periodic_tasks();
static void TextSpicyv4();

//------------------------core1--------------------------------
void setup()
{
  print_startup_message();
  // rp2040.wdt_begin(WATCHDOG_TIMEOUT);
  // oxy_setup();
}

void loop()
{
  check_periodic_tasks();
  // Serial.print(oxy_isconnected());
  // Serial.println();
  // delay(100);

  temp_record_temp("temp_reading.csv",NTC_0, NTC_2, 1000 * 60);

  if (temp_read_one(NTC_0) > 30.0)
  {
    heat_updateone(PIN_H0, 0);
  }
  else
  {
    heat_updateone(PIN_H0, 100.0);
  }
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
void setup1()
{
}

void loop1()
{
  // pid_update_all();
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

  TextSpicyv4();

  if (watchdog_caused_reboot())
  {
    debugf_status(">[MotherboardV4.ino] is running on Chip %i Core %i |Freq %.1f MHz|Watchdog Reset<<\n", rp2040.getChipID(), rp2040.cpuid(), rp2040.f_cpu() / 1000000.0);
  }
  else
  {
    debugf_status(">[MotherboardV4.ino] is running on Chip %i Core %i |Freq %.1f MHz<<\n", rp2040.getChipID(), rp2040.cpuid(), rp2040.f_cpu() / 1000000.0);
  }

  debugf_info("\"/?\" for help\n");
  rp2040.enableDoubleResetBootloader();
#endif
}

/**
 *Serial.prints the SpiCy Logo.
 *source: https://patorjk.com/software/taag/#p=display&f=Graffiti&t=Type%20Something%20
 */
void TextSpicyv4()
{
#if DEBUG == 1
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

#endif
}
