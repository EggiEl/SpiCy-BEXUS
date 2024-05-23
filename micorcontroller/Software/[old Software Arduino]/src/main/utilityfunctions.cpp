#include "header.h"
#define R_SHUNT 1 // in mOhms
#define nGPIOS 29

void printMemoryUse();
void printIO(uint8_t anzahl_Ios);
void short_detection();

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

void printIO()
{
  Serial.println();
  Serial.println("printio: ");
  debug("Pinnumb: ");
  char *d = (char *)malloc(nGPIOS);
  char *a = (char *)malloc(nGPIOS);
  if (d == NULL)
  {
    Serial.println("Initializing of dynamic array d failed");
  }
  if (a == NULL)
  {
    Serial.println("Initializing of dynamic array a failed");
  }
  for (int i = 0; i < nGPIOS; i++)
  {
    pinMode(i, INPUT);
    d[i] = digitalRead(i);
    a[i] = analogRead(i);
  }
  for (int i = 0; i < nGPIOS; i++)
  {
    debug(i);
    if (i < 10)
    {
      debug(" ");
    }
    debug(" |");
  }
  Serial.println();
  debug("Digital: ");
  for (int i = 0; i < nGPIOS; i++)
  {
    debug(d[i], BIN);
    debug(" ");
    debug(" |");
  }
  Serial.println();
  debug("Analog:  ");
  for (int i = 0; i < nGPIOS; i++)
  {
    debug(a[i], DEC);
    if (a[i] < 100)
    {
      debug(" ");
    }
    debug("|");
  }
}

/*Reads Serial Commands wich start with "/". /? for help */
void serial_commands()
{
#if DEBUG == 1
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
/w|Sets Watchdog to ... ms. Cant be disables till reboot.\n\
/u|single file usb update. /u 1 closes singlefileusb\n");
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
        rp2040.idleOtherCore();
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
        rp2040.resumeOtherCore();
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
      case 'u':
      {
        if (Serial.parseInt(SKIP_WHITESPACE) == 1)
        {
          singlefile_close();
        }
        else
        {
          usb_singlefile_update();
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
#endif
}

void short_detection()
{
  debugln("<shorted_pin_detection>");
  char *pin_buffer = (char *)calloc(nGPIOS + 1, 1);

  // shorts to ground detection
  for (int i = 0; i <= nGPIOS + 1; i++)
  {
    pinMode(i, INPUT_PULLUP);
  }
  for (int i = 0; i <= nGPIOS + 1; i++)
  {
    if (digitalRead(i) == 0)
    {
      debug("Pin ");
      debug(i);
      debug(" is connected to ground.\n");
    }
  }

  // shorts to VCC detection
  for (int i = 0; i <= nGPIOS; i++)
  {
    pinMode(i, INPUT_PULLDOWN);
  }
  for (int i = 0; i <= nGPIOS; i++)
  {
    if (digitalRead(i) == 1)
    {
      debug("Pin ");
      debug(i);
      debug(" is connected to VCC. I2C?.\n");
      pin_buffer[i] = 1;
    }
  }

  // pin shorting each other
  for (int i = 0; i <= nGPIOS; i++)
  {
    pinMode(i, INPUT_PULLDOWN);
  }

  for (int Pin_now = 0; Pin_now <= nGPIOS; Pin_now++)
  {
    if (pin_buffer[Pin_now] == 1)
    {
    }
    else
    {
      // sets all pins to 0
      for (int i = 0; i <= nGPIOS; i++)
      {
        digitalWrite(i, 0);
      }

      // tests for a short by individually setting each pin high
      for (int i = 0; i <= nGPIOS; i++)
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
  }
  free(pin_buffer);
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

/*
void sleep_goto_sleep_until(datetime_t *t, rtc_callback_t callback)
{
  // We should have already called the sleep_run_from_dormant_source function
  assert(dormant_source_valid(_dormant_source));

  // Turn off all clocks when in sleep mode except for RTC
  clocks_hw->sleep_en0 = CLOCKS_SLEEP_EN0_CLK_RTC_RTC_BITS;
  clocks_hw->sleep_en1 = 0x0;

  rtc_set_alarm(t, callback);

  uint save = scb_hw->scr;
  // Enable deep sleep at the proc
  scb_hw->scr = save | M0PLUS_SCR_SLEEPDEEP_BITS;

  // Go to sleep
  __wfi();
}
*/

/*
void sleep_goto_dormant_until_pin(uint gpio_pin, bool edge, bool high) {
 bool low = !high;
 bool level = !edge;

 // Configure the appropriate IRQ at IO bank 0
 assert(gpio_pin < NUM_BANK0_GPIOS);

 uint32_t event = 0;

 if (level && low) event = IO_BANK0_DORMANT_WAKE_INTE0_GPIO0_LEVEL_LOW_BITS;
 if (level && high) event = IO_BANK0_DORMANT_WAKE_INTE0_GPIO0_LEVEL_HIGH_BITS;
 if (edge && high) event = IO_BANK0_DORMANT_WAKE_INTE0_GPIO0_EDGE_HIGH_BITS;
 if (edge && low) event = IO_BANK0_DORMANT_WAKE_INTE0_GPIO0_EDGE_LOW_BITS;

 gpio_set_dormant_irq_enabled(gpio_pin, event, true);

_go_dormant();

 // Execution stops here until woken up

 // Clear the irq so we can go back to dormant mode again if we want
 gpio_acknowledge_irq(gpio_pin, event);
 }
*/
