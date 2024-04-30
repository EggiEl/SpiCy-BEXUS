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
  print_startup_message();
}

void loop()
{
  check_periodic_tasks();
}

//-------------------------core2------------------------
void setup1()
{

}

void loop1()
{
  void fadeLED();
}

void check_periodic_tasks()
{
  checkSerialInput();
  rp2040.wdt_reset();
}

void print_startup_message()
{
#if DEBUG == 1
  Serial.setTimeout(1000);
  Serial.begin(460800);
  // Serial.begin(115200);

  while (!Serial)
  {
  }
  delay(50);
  TextSpicyv4();

  debugf_yellow("\n<<[MotherboardV4.ino] is running on Chip %i>>\n Core %i |Freq %.1f ", rp2040.getChipID(), rp2040.cpuid(), rp2040.f_cpu() / 1000000.0);
  if (watchdog_caused_reboot())
  {
    debugf_yellow("MHz|Watchdog Reset>>\n");
  }
  else
  {
    debugf_yellow("MHz\n");
  }
  debugf_yellow("\"/?\" for help\n");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
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
  if (random(3) != 1)
  {
    // ghost
    debugf_green(
        "  .-')       _ (`-.                                                 (`-.\n"
        " ( OO ).    ( (OO  )                                              _(OO  )_\n"
        "(_)---\\_)  _.`     \\   ,-.-')     .-----.    ,--.   ,--.      ,--(_/   ,. \\    .---.\n"
        "/    _ |  (__...--''   |  |OO)   '  .--./     \\  `.'  /       \\   \\   /(__/   / .  |\n"
        "\\  :` `.   |  /  | |   |  |  \\   |  |('-.   .-')     /         \\   \\ /   /   / /|  |\n"
        " '..`''.)  |  |_.' |   |  |(_/  /_) |OO  ) (OO  \\   /           \\   '   /,  / / |  |_\n"
        ".-._)   \\  |  .___.'  ,|  |_.'  ||  |`-'|   |   /  /\\_           \\     /__)/  '-'    |\n"
        "\\       /  |  |      (_|  |    (_'  '--'\\   `-./  /.__)           \\   /    `----|  |-'\n"
        " `-----'   `--'        `--'       `-----'     `--'                 `-'          `--'");
  }
  else
  {
    // graphiti
    SET_COLOUR_GREEN
    Serial.println(" _________        .__ _________                   _____  ");
    Serial.println("/   _____/______  |__|\\_   ___ \\ ___.__. ___  __ /  |  | ");
    Serial.println("\\_____  \\ \\____ \\ |  |/    \\  \\/<   |  | \\  \\/ //   |  |");
    Serial.println("/        \\|  |_> >|  |\\     \\____\\___  |  \\   //    ^   /");
    Serial.println("/_______  /|   __/ |__| \\______  // ____|   \\_/ \\____   | ");
    Serial.println("        \\/ |__|                \\/ \\/                 |__|");
    SET_COLOUR_RESET
  }
#endif
}
