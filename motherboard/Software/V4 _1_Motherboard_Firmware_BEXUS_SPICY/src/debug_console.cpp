#include "header.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#define nGPIOS 29
static void printIO();
static void short_detection();
void printMemoryUse();

/**
 * Reads Serial Commands wich start with "/".
 * /? for help
 *disabled if DEBUG == 0
 */
void checkSerialInput()
{
#if DEBUG == 1
  if (Serial.available())
  {
    if (Serial.read() == '/')
    {

      char buffer_comand = Serial.read();
      float param1 = -1;
      float param2 = -1;
      float param3 = -1;
      float param4 = -1;
      if (Serial.available() >= 4) // min. 4 bytes to get a float
      {
        param1 = Serial.parseFloat(SKIP_WHITESPACE);
        if (Serial.available() >= 4)
        {
          param2 = Serial.parseFloat(SKIP_WHITESPACE);

          if (Serial.available() >= 4)
          {
            param3 = Serial.parseFloat(SKIP_WHITESPACE);

            if (Serial.available() >= 4)
            {
              param4 = Serial.parseFloat(SKIP_WHITESPACE);
            }
          }
        }
      }

      handleCommand(buffer_comand, param1, param2, param3, param4);
    }
  }
#endif
}

/**
 *
 */
void handleCommand(char buffer_comand, float param1, float param2, float param3, float param4)
{
  switch (buffer_comand)
  {
  case '?':
  {
    debugf_status("<help>\n");
    debugln(F(
        "/b|Returns Battery Voltage and current\n\
/s|Read out Status\n\
/l|Sets the controller in sleep for ... ms.\n\
/r|Reboots. if followed by a 1 reboots in Boot Mode\n\
/m|Read out Memory Info\n\
/h x y| set heater pin x at prozent y .if x == -1 is given\n\
        will set all heater to different numbers for testing.\n\
/i|scans for i2c devices\n\
/f|changes the analogWrite frequency. \n\
   For heater run one heating command and change then.\n\
/d|pin shorts detection\n\
/p|sends a test packet over lan\n\
/w|Sets Watchdog to ... ms. Cant be disables till reboot.\n\
/u|single file usb update. /u 1 closes singlefileusb\n\
/t|returns temperature value. 0-6 for external probes, 7 for uC one, -1 for all of them\n\
/x|prints barometer temperature and pressure\n\
/o|starts console to talk to PyroSience FD-OEM Oxygen Module\n\
/a|reads out light spectrometers\n"));
    break;
  }
  case 'b':
  {
    debugf_status("BatteryVoltage: %.4f V| Current: %.4f mA, %.2f A\n", get_batvoltage(), get_current() * 1000, get_current());
    break;
  }
  case 's':
  {
    debugf_status("<get Status>\n");
    debugf_sucess("Status: %i\n", get_Status());
    break;
  }
  case 'r':
  {
    if (param1 == 1)
    {
      debugf_magenta("<Reboot in Boot Mode>\n");
      rp2040.rebootToBootloader();
      break;
    }
    else
    {
      debugf_magenta("<Reboot>\n");
      rp2040.reboot();
      break;
    }
  }
  case 'l':
  {
    rp2040.idleOtherCore();
    debugf_status("RP2040 sleepy for %.2fs.", param1 / 1000.0);
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
    delay(param1);
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
    if (param1 == -1)
    {
      heat_testmanual();
      break;
    }
    else
    {
      debugf_status("Update Heater Pin %i to %i%\n", (int)param1, (int)param2);
      heat_updateone((int)param1, (int)param2);
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
    if (param1)
    {
      debug("Set Freq of AnalogWrite to: ");
      debugln((int)param1);
      // ADC_FREQ_WRITE = (int)param1;
      analogWriteFreq(param1);
    }
    else
    {
      debug("Numerical input >= 0");
    }
    break;
  }
  case 'd':
  {
    debugf_status("<shorted_pin_detection>\n");
    short_detection();
    break;
  }
  case 'p':
  {
    if (param1)
    {
      debugf_status("<Sending %.f Test Packets>\n", param1);
      tpc_testmanually(param1);
    }
    else
    {
      debugf_status("<Sending one Test Packet>\n");
      tpc_testmanually(1);
    }
    break;
  }
  case 'w':
  {
    rp2040.wdt_reset();
    if (param1 <= 8300)
    {
      rp2040.wdt_reset();
      rp2040.wdt_begin(param1);
      debugf_status("Set watchdog to %.2fs\n", param1 / 1000.0);
    }
    else
    {
      debugf_status("Watchdog value to high, 8.3 seconds are the maximum\n");
    }
    break;
  }
  case 'u':
  {
#if USB_ENABLE == 1
    if (param1 == 1)
    {
      singlefile_close();
    }
    else
    {
      usb_singlefile_update();
    }
#else
    debugf_status("Single file Usb disabled\n");
#endif
    break;
  }
  case 't':
  {
    if (param1 == -1)
    {
      debugf_status("<Reading out Thermistors>\n");
      float buf[8];
      temp_read_all(buf);
      for (int i = 0; i < 6; i++)
      {
        debugf_info("Probe Nr:%i|%.2f°C\n", i, buf[i]);
      }
      debugf_info("Probe SMD|%.2f°C\n", buf[6]);
      debugf_info("Probe Check|%.2f°C [should be 25°C]\n", buf[7]);
    }
    else
    {
      debugf_status("<Reading out Thermistor %u>\n", (unsigned int)param1);
      temp_read_one((unsigned int)param1);
    }
    break;
  }
  case 'x':
  {
    debugf_status("<Reads out Barometer> WIP\n");
    // pressure_read();
    break;
  }
  case 'o':
  {
    oxy_console();
    break;
  }
  case 'a':
  {
    float buffer[7];
    light_read(buffer, 0);
    break;
  }
  default:
  {
    debugf_error("dafuck is \"/%c\" ?!?! try \"/?\".\n", buffer_comand);
    break;
  }
  }
}

/*swtiches LED*/
void StatusLedBlink(uint8_t LED)
{
  // static unsigned int T_ms = 1;
  // static unsigned long timestamp = millis() + T_ms;
  // if (millis() > timestamp)
  // {
  if (digitalRead(LED))
  {
    digitalWrite(LED, 0);
  }
  else
  {
    digitalWrite(LED, 1);
  }
  // }
}

// const float R8 = 100000.0;
// const float R9 = 10000.0;
// const float BAT_VOLTAG_DIV = R9/(R9+R8); /*Votage Divider from wich the battery volage can be calculated*/
const float BAT_VOLTAG_DIV = 0.09269136808;
/*returns the battery voltage at the moment in V*/
float get_batvoltage()
{
  analogReadResolution(ADC_RES);
  pinMode(PIN_VOLT, INPUT);
  float adc_volt = (float)(analogRead(PIN_VOLT) / ADC_MAX_READ) * ADC_REF;
  // debugf_info("batmes_adc_volt:%.4f\n", adc_volt);
  return adc_volt / BAT_VOLTAG_DIV;
}

const float R_SHUNT = 1.0; // in mOhms
/*returns the current consumption at the moment of the PCB in A*/
float get_current()
{
  analogReadResolution(ADC_RES);
  pinMode(PIN_CURR, INPUT);
  float adc_volt = (float)(analogRead(PIN_CURR) / ADC_MAX_READ) * ADC_REF;
  // debugf_info("batmes_adc_curr:%.4f\n", adc_volt);
  return adc_volt / (R_SHUNT * 2.5);
}

/*print current Stack/Heap use*/
void printMemoryUse()
{
  debugf_info("-freeStack: %.2f kbytes\n", (float)rp2040.getFreeStack() * 0.001);
  debugf_info("-freeHeadp: %.2f kbytes | usedHeap: %.2f kbytes\n", (float)rp2040.getFreeHeap() * 0.001, (float)rp2040.getUsedHeap() * 0.001);
}

/*DIgital and Analog Reads all IO Pins*/
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

/*needs rework to exclude Flash*/
void short_detection()
{
  char *pin_buffer = (char *)calloc(nGPIOS + 1, 1);

  // shorts to ground detection
  for (int i = 0; i < nGPIOS + 1; i++)
  {
    pinMode(i, INPUT_PULLUP);
  }
  for (int i = 0; i < nGPIOS + 1; i++)
  {
    if (digitalRead(i) == 0)
    {
      debug("Pin ");
      debug(i);
      debug(" is connected to ground.\n");
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
      debug(" is connected to VCC. I2C?.\n");
      pin_buffer[i] = 1;
    }
  }

  // pin shorting each other
  for (int i = 0; i < nGPIOS; i++)
  {
    pinMode(i, INPUT_PULLDOWN);
  }

  for (int Pin_now = 0; Pin_now < nGPIOS; Pin_now++)
  {
    if (pin_buffer[Pin_now] == 1)
    {
    }
    else
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
  }
  free_ifnotnull(pin_buffer);
  debugln("Short detection done.");
}

/*frees given pointer it it isnt NULL, then sets it to NULL.
 If it is NULL, throws error*/
void free_ifnotnull(void *pointer)
{
  if (pointer != NULL)
  {
    free(pointer);
    pointer = NULL;
  }
  else
  {
    debugf_warn("Tried to free pointer wich was freed bevore\n");
  }
}

/**
 * returns a status value wich can be decoded to get infos about the parts of the module
 *  @return uint32_t status bitwise encoded as:
 *  | default 1 | sd init | cableTest  | TCP init | Heater init | Oxy init | Light init |
 *        0         1          2           3             4           5             6
 */
uint32_t get_Status()
{
  uint32_t status = 0;
  status = 1;

  // sd
  sd_setup();
  status |= ((uint32_t)sd_init << 1);

  // TCP connection
  status |= ((uint32_t)tcp_link_status() << 2);

  tcp_setup_client();
  status |= ((uint32_t)TCP_init << 3);

  // Heater
  heat_setup();
  status |= ((uint32_t)heat_init << 4);

  // Oxygen Sensors
  oxy_setup();
  status |= ((uint32_t)oxy_init << 5);

  // Light
  light_setup();
  status |= ((uint32_t)light_init << 6);

  // memory
  return status;
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