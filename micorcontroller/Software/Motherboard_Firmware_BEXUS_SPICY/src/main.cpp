/*-------------Main programm coordinating the different sections-------------------*/
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
#if DEBUG == 1
  delay(60 * 60 * 1000);
  debugln("<<rebooted to bootloader after on hour>>");
  rp2040.rebootToBootloader();
#endif
  // debugln("----------Main is running-C1-------------------");
}

void loop1() {}

void check_periodic_tasks()
{
  serial_commands();
  rp2040.wdt_reset();
}

void print_startup_message()
{
#if DEBUG == 1
  Serial.begin(460800);
  // Serial.begin(115200);
  Serial.setTimeout(1000);
  while (!Serial)
  {
  }
  delay(50);
  TextSpicyv4();
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

/*Serial.prints the SpiCy Logo*/
void TextSpicyv4()
{
  if (random(3) == 1)
  {
    // ghost
    Serial.println(
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
    Serial.println(" _________        .__ _________                   _____  ");
    Serial.println("/   _____/______  |__|\\_   ___ \\ ___.__. ___  __ /  |  | ");
    Serial.println("\\_____  \\ \\____ \\ |  |/    \\  \\/<   |  | \\  \\/ //   |  |");
    Serial.println("/        \\|  |_> >|  |\\     \\____\\___  |  \\   //    ^   /");
    Serial.println("/_______  /|   __/ |__| \\______  // ____|   \\_/ \\____   | ");
    Serial.println("        \\/ |__|                \\/ \\/                 |__|");
  }
}
