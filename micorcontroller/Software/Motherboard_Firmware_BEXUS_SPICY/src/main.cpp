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

// https://arduino-pico.readthedocs.io/en/latest/index.html
void print_startup_message();
void check_periodic_tasks();
static void TextSpicyv4();

//------------------------core1--------------------------------
void setup()
{
  heat_setup();
  print_startup_message();
  rp2040.wdt_begin(8000);
}

void loop()
{
  nextState();
  check_periodic_tasks();
}

//-------------------------core2------------------------
void setup1()
{
  pinMode(LED_BUILTIN, 1);
  pinMode(STATLED_R, OUTPUT);
  pinMode(STATLED_G, OUTPUT);
  pinMode(STATLED_B, OUTPUT);
  digitalWrite(STATLED_R, 1);
  digitalWrite(STATLED_G, 1);
  digitalWrite(STATLED_B, 1);
}

void loop1()
{
  StatusLed_Downlink();
}

/*all things that should get checkt every loop of CPU0*/
void check_periodic_tasks()
{
  checkSerialInput();
  // tcp_check_command();
  rp2040.wdt_reset();
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

  debugf_status("\n<<[MotherboardV4.ino] is running on Chip %i>>\n Core %i |Freq %.1f ", rp2040.getChipID(), rp2040.cpuid(), rp2040.f_cpu() / 1000000.0);
  if (watchdog_caused_reboot())
  {
    debugf_status("MHz|Watchdog Reset>>\n");
  }
  else
  {
    debugf_status("MHz\n");
  }
  debugf_status("\"/?\" for help\n");
  pinMode(STATLED_R, OUTPUT);
  digitalWrite(STATLED_R, HIGH);
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
      " `-----'   `--'        `--'       `-----'     `--'        ");
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
