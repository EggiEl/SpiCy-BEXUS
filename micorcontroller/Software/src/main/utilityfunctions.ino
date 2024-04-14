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

/*print current Stack/Heap use*/
void printMemoryUse()
{
  int totalHeap = rp2040.getTotalHeap();
  debug("-TotalHeap: ");
  debug(rp2040.getTotalHeap() * 0.001);
  debugln(" kbytes");

  debug("-FreeHeap: ");
  debug(rp2040.getFreeHeap() * 0.001);
  debug(" kbytes| ");
  debug((float)rp2040.getFreeHeap() / totalHeap);
  debugln(" percent");

  debug("-UsedHeap: ");
  debug(rp2040.getUsedHeap() * 0.001);
  debug(" kbytes| ");
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
    char buffer_comand = Serial.read();
    if (buffer_comand == '/')
    {
      int buffer; // multipurpose buffer pirmarily for inputs after command
      buffer_comand = Serial.read();
      switch (buffer_comand)
      {
      case '?':
      {
        debugln("<help>\n\
please add to every numerical input one, it will be automaticly subtracted\n\
/b|Returns Battery Voltage and current\n\
/s|Read out Status\n\
/l|Sets the controller in sleep for ... ms.\n\
/r|Reboots. if followed by a 1 reboots in Boot Mode\n\
/m|Read out Memory Info\n\
/h x y| set heater pin x at prozent y . if pin_numb > 256 is given will set all heater to different numbers for testing.\n\
/i|scans for i2c devices\n\
/f|changes the analogWrite frequency. For heater run one heating command and change then.\n\
/c|prints current clock speed of the RP2040\n\
/d|pin shorts detection\n\
/p|sends a test packet over lan\n\
/w|Sets Watchdog to ... ms. Cant be disables till reboot.");
        break;
      }
      case 'b':
      {
        debug("BatteryVoltage: ");
        debug(get_batvoltage() * 1000.0);
        debug("V, Current: ");
        debug(get_current() * 1000.0);
        debugln("A");
        break;
      }
      case 's':
      {
        uint32_t buffer_status = get_Status();
        debug("Status: ");
        debugln(buffer_status);
        break;
      }
      case 'r':
      {
        buffer = Serial.parseInt(SKIP_WHITESPACE);
        if (buffer == 1)
        {
          debugln("-Reboot in Boot Mode-");
          rp2040.rebootToBootloader();
          break;
        }
        else
        {
          debugln("-Reboot-");
          rp2040.reboot();
          break;
        }
      }
      case 'l':
      {

        buffer = Serial.parseInt(SKIP_WHITESPACE);
        debug("RP2040 sleepy for ");
        debug(buffer / 1000.0);
        debugln("s.");
        // // Calculate the number of microseconds to sleep
        // uint64_t microseconds = (uint64_t)buffer * 1000;
        // // Clear any existing alarm interrupts
        // hw_clear_bits(&timer_hw->inte, TIMER_INTF_RESET);
        // // Split the 64-bit value into two 32-bit parts and set the timer alarm
        // uint32_t alarm_low = (uint32_t)microseconds;
        // uint32_t alarm_high = (uint32_t)(microseconds >> 32);
        // timer_hw->alarm = alarm_low;
        // timer_hw->alarm_hi = alarm_high;
        // // Enable the timer interrupt
        // hw_set_bits(&timer_hw->inte, TIMER_INTF_ALARM);
        // // Enter sleep mode (replace with appropriate Arduino sleep function if using Arduino)
        // // sleep_cpu();
        // // For Arduino, you can use delay() or other sleep functions provided by Arduino libraries
        delay(buffer);
        debugln("Awake Again");
        break;
      }
      case 'm':
      {
        printMemoryUse();
        break;
      }
      case 'h':
      {
        int pin_numb = Serial.parseInt(SKIP_WHITESPACE);      // saves heater number in the buffer_command
        uint16_t pwm_duty = Serial.parseInt(SKIP_WHITESPACE); // saves heater power  in the pwm_duty
        if (pin_numb > 256)
        {
          heat_testmanual();
          break;
        }
        debug("Update Heater ");
        debug(pin_numb - 1);
        debug(" to ");
        debugln(pwm_duty - 1);
        if (pin_numb && pwm_duty)
        {
          heat_updateone(pin_numb - 1, pwm_duty - 1);
        }
        else
        {
          debugln("Please Numbers above 0.");
        }
        break;
      }
      case 'i':
      {
        scan_wire();
        break;
      }
      case 'f':
      {
        int freq = Serial.parseInt(SKIP_WHITESPACE);
        if (freq)
        {
          debug("Set Freq of AnalogWrite to: ");
          debugln(freq);
          analogWriteFreq(freq);
          analogWriteRange(HEAT_HUNDERTPERCENT);
        }
        else
        {
          debug("Numerical input >= 0");
        }

        break;
      }
      case 'c':
      {
        debug("System freuqncy: ");
        debugln(rp2040.f_cpu());
        break;
      }
      case 'd':
      {
        short_detection();
        break;
      }
      case 'p':
      {
        debugln("<Sending a Test Packet with info \"testlö\".>");
        struct packet test = packet_create();
        packet_writeinfo(test, "testlö");
        send_TCP_packet(test);
      }
      case 'w':
      {
        rp2040.wdt_reset();
        unsigned int buffer_time = Serial.parseInt(SKIP_WHITESPACE);
        if (buffer_time <= 8300)
        {
          rp2040.wdt_reset();
          rp2040.wdt_begin(buffer_time);
          debug("Set watchdog to ");
          debug(buffer_time / 1000.0);
          debugln("s.");
        }
        else
        {
          debugln("Watchdog value to high, 8.3 seconds are the maximum");
        }
        break;
      }
      default:
      {
        debug("dafuck is \"");
        debug(buffer_comand);
        debugln("\" ?!?! try \"/?\" or switching to BWL.\n");
        break;
      }
      }
    }
  }
}

#define nGPIOS 30
void short_detection()
{
  debugln("<shorted_pin_detection>");

  // shorts to ground detection
  for (int i = 0; i < nGPIOS; i++)
  {
    pinMode(i, INPUT_PULLUP);
  }
  for (int i = 0; i < nGPIOS; i++)
  {
    if (digitalRead(i) == 0)
    {
      debug("Pin ");
      debug(i);
      debug(" is shorted to ground.");
      debug(" |AnalogRead = ");
      debugln(analogRead(i));
    }
  }

  // shorts to VCC detection
  for (int i = 0; i < nGPIOS; i++)
  {
    pinMode(i, INPUT_PULLDOWN);
  }
  for (int i = 0; i < nGPIOS; i++)
  {
    if (digitalRead(i) == 1)
    {
      debug("Pin ");
      debug(i);
      debug(" is shorted to VCC.");
      debug(" |AnalogRead = ");
      debugln(analogRead(i));
    }
  }

  // pin shorting each other
  for (int i = 0; i < nGPIOS; i++)
  {
    pinMode(i, INPUT_PULLDOWN);
  }

  for (int Pin_now = 0; Pin_now < nGPIOS; Pin_now++)
  {
    // sets all pins to 0
    for (int i = 0; i < nGPIOS; i++)
    {
      digitalWrite(i, 0);
    }

    // tests for a short by individually setting each pin high
    for (int i = 0; i < nGPIOS; i++)
    {
      if (i != Pin_now)
      {
        digitalWrite(i, HIGH);
        if (digitalRead(Pin_now) == 1)
        {
          debug("Pin ");
          debug(Pin_now);
          debug(" is shorted to ");
          debugln(i);
        }
        digitalWrite(i, LOW);
      }
    }
  }
  debugln("Short detection done.");
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