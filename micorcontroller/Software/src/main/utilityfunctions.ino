#define STATLED_R 23 // Status LeD pins
#define STATLED_G 22
#define STATLED_B 21

#define PIN_VOLT A3
#define PIN_CURR A2
#define R_SHUNT 1 // in mOhms

// returns the battery voltage at the moment in mV
unsigned long get_batvoltage()
{
  pinMode(PIN_CURR, OUTPUT);
  return analogRead(PIN_VOLT) * 1000 * 5;
}

// returns the current consumption at the moment of the PCB in mA
unsigned long get_current()
{
  pinMode(PIN_CURR, OUTPUT);
  float buf = analogRead(PIN_CURR) * (3.3 / 4.0950); // 4.0950 instead of 4095.0 because of the convertion of A to mA
  return buf / (R_SHUNT * 2.5);
}

void printMemoryUse()
{
  int totalHeap = rp2040.getTotalHeap();
  debug("-TotalHeap: ");
  debug(rp2040.getTotalHeap());
  debugln(" bytes");

  debug("-FreeHeap: ");
  debug(rp2040.getFreeHeap());
  debug(" bytes| ");
  debug((float)rp2040.getFreeHeap() / totalHeap);
  debugln(" percent");

  debug("-UsedHeap: ");
  debug(rp2040.getUsedHeap());
  debug(" bytes| ");
  debug((float)rp2040.getUsedHeap() / totalHeap);
  debugln(" percent");
}

void printIO(uint8_t anzahl_Ios)
{
  Serial.println();
  Serial.println("printio: ");
  Serial.print("Pinnumb: ");
  char *d = (char *)malloc(anzahl_Ios);
  char *a = (char *)malloc(anzahl_Ios);
  if (d == NULL)
  {
    Serial.println("Initializing of dynamic array d failed");
  }
  if (a == NULL)
  {
    Serial.println("Initializing of dynamic array a failed");
  }
  for (int i = 0; i < anzahl_Ios; i++)
  {
    pinMode(i, INPUT);
    d[i] = digitalRead(i);
    a[i] = analogRead(i);
  }
  for (int i = 0; i < anzahl_Ios; i++)
  {
    Serial.print(i);
    if (i < 10)
    {
      Serial.print(" ");
    }
    Serial.print(" |");
  }
  Serial.println();
  Serial.print("Digital: ");
  for (int i = 0; i < anzahl_Ios; i++)
  {
    Serial.print(d[i], BIN);
    Serial.print(" ");
    Serial.print(" |");
  }
  Serial.println();
  Serial.print("Analog:  ");
  for (int i = 0; i < anzahl_Ios; i++)
  {
    Serial.print(a[i], DEC);
    if (a[i] < 100)
    {
      Serial.print(" ");
    }
    Serial.print("|");
  }
}

/*Reads Serial Commands wich start with "/". /? for help */
void serial_commands()
{
  if (Serial.available() > 0)
  {
    uint32_t buffer_status;
    char buffer_comand = Serial.read();
    if (buffer_comand == '/')
    {
      int buffer; // multipurpose buffer pirmarily for inputs after command
      buffer_comand = Serial.read();
      switch (buffer_comand)
      {
      case '?':
        debugln("<help>");
        debugln("/p|Returns Battery Voltage and current");
        debugln("/s|Read out Status");
        debugln("/l|Sets the controller in deep sleep");
        debugln("/r|Reboots. if followed by a 1 reboots in Boot Mode");
        debugln("/m|Read out Memory Info");
        debugln("/h x y| set heater x at y prozent");
        break;

      case 'p':
        debug("BatteryVoltage: ");
        debug(get_batvoltage() * 1000.0);
        debug("V, Current: ");
        debug(get_current() * 1000.0);
        debugln("A");
        break;

      case 's':
        buffer_status = get_Status();
        debug("Status: ");
        debugln(buffer_status);
        break;

      case 'r':
        buffer = Serial.parseInt(SKIP_WHITESPACE);
        if (buffer == 1)
        {
          debugln("-Reboot in Boot Mode-");
          rp2040.rebootToBootloader();
          break;
        }
        else{
            debugln("-Reboot-");
          rp2040.reboot();
          break;
        }

      case 'l':
        debugln("Entering Sleep");
        buffer = Serial.parseInt(SKIP_WHITESPACE) * 1000;
        // sleep_config_set_default();
        // // Set wake-up source to time
        // sleep_configure_wakeup(
        //     rtc_time_ms() + 5000, // Wake up after 5 seconds
        //     0,                    // Don't repeat
        //     false                 // Wake up on time, not alarm
        // );
        // // Enter deep sleep mode
        // sleep_manager_deep_sleep();
        debugln("Awake Again");
        break;

      case 'm':
        printMemoryUse();
        break;

      case 'h':
        debug("-Heater ");
        uint16_t buffer_heat_pw;
        uint8_t buffer_heat_pin;
        // wich heater? saves in the buffer_command
        buffer_heat_pin = Serial.read();
        switch (buffer_heat_pin)
        {
        case 0:
          debugln("no heater value given-");
          break;
        default:
          debug("Heater selected: ");
          debug(buffer_heat_pin);

          // how mutch power? saves the value in buffer
          buffer_heat_pw = Serial.parseInt(SKIP_WHITESPACE);
          switch (buffer_heat_pw)
          {
          case 0:
            debugln("no power value given");
            break;
          default:
            debug(" is set to: ");
            debug(buffer_heat_pw);
            heat_updateone(buffer_heat_pin, buffer_heat_pw);
            break;
          }
          break;
        }

      default:
        debugln("Command invalid\n");
        break;
      }
    }
  }
}

void StatusLedBlink()
{
  pinMode(STATLED_R, OUTPUT);
  pinMode(STATLED_G, OUTPUT);
  pinMode(STATLED_B, OUTPUT);

  digitalWrite(STATLED_R, HIGH);
  delay(200);
  digitalWrite(STATLED_G, HIGH);
  delay(200);
  digitalWrite(STATLED_B, HIGH);
  delay(200);
  digitalWrite(STATLED_R, LOW);
  delay(200);
  digitalWrite(STATLED_G, LOW);
  delay(200);
  digitalWrite(STATLED_B, LOW);
  delay(200);
}

void blinkLed()
{
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
}

void fadeLED()
{
  analogWriteRange(10000);
  for (int brightness = 0; brightness < 5000; brightness++)
  {
    analogWrite(LED_BUILTIN, brightness);
    delay(1);
  }

  // Fade out
  for (int brightness = 2500; brightness >= 0; brightness--)
  {
    analogWrite(LED_BUILTIN, brightness);
    delay(1);
  }
}

void heartbeat()
{
  for (int i = 100; i >= 10; i -= 10)
  {
    analogWrite(LED_BUILTIN, 255); // Turn the LED on
    delay(i ^ 2);                  // Wait
    analogWrite(LED_BUILTIN, 0);   // Turn the LED off
    delay(i ^ 2);                  // Wait
  }

  // Slowly decrease the heartbeat rate
  for (int i = 10; i <= 100; i += 10)
  {
    analogWrite(LED_BUILTIN, 255); // Turn the LED on
    delay(i ^ 2);                  // Wait
    analogWrite(LED_BUILTIN, 0);   // Turn the LED off
    delay(i ^ 2);                  // Wait
  }
}