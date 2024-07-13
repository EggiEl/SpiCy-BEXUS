#include "header.h"
#include "debug_in_color.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#define nGPIOS 29
uint32_t check_peripherals();

/**
 * Reads Serial Commands wich start with "/".
 * /? for help
 *disabled if DEBUG_MODE == 0
 */
void checkSerialInput()
{
#if DEBUG_MODE == 1
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
/d|check connected peripherals\n\
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
      switch ((uint)param1)
      {
      case 0:
        debugf_status("Update Heater %u to %.2f %\n", (uint)param1, param2);
        heat_updateone(PIN_H0, param2);
        break;
      case 1:
        debugf_status("Update Heater %u to %.2f %\n", (uint)param1, param2);
        heat_updateone(PIN_H1, param2);
        break;
      case 2:
        debugf_status("Update Heater %u to %.2f %\n", (uint)param1, param2);
        heat_updateone(PIN_H2, param2);
        break;
      case 3:
        debugf_status("Update Heater %u to %.2f %\n", (uint)param1, param2);
        heat_updateone(PIN_H3, param2);
        break;
      case 4:
        debugf_status("Update Heater %u to %.2f %\n", (uint)param1, param2);
        heat_updateone(PIN_H4, param2);
        break;
      case 5:
        debugf_status("Update Heater %u to %.2f %\n", (uint)param1, param2);
        heat_updateone(PIN_H5, param2);
        break;
      case 6:
        debugf_status("Update Heater %u to %.2f %\n", (uint)param1, param2);
        heat_updateone(PIN_H6, param2);
        break;
      case 7:
        debugf_status("Update Heater %u to %.2f %\n", (uint)param1, param2);
        heat_updateone(PIN_H7, param2);
        break;
      default:
        debugf_status("Invalid Heater ID: %i\n", (uint)param1);
        break;
      }
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
    debugf_status("<pin_detection>\n");
    check_peripherals();
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
      debugf_info("%f °C\n", temp_read_one((unsigned int)param1));
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

/*swtiches LED in fixed frequency. Non blocking*/
void StatusLedBlink(uint8_t LED)
{
  static unsigned int T_ms = 100;
  static unsigned long timestamp = millis() + T_ms;
  if (millis() > timestamp)
  {
    timestamp = millis() + T_ms;
    static uint8_t status = 0;
    pinMode(LED, OUTPUT);
    if (status)
    {
      status = 0;
      digitalWrite(LED, 1);
    }
    else
    {
      status = 1;
      digitalWrite(LED, 0);
    }
  }
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
  float analog_read_buffer = 0;
  for (int i = 0; i < 50; i++)
  {
    analog_read_buffer += analogRead(PIN_VOLT);
  }
  analog_read_buffer = analog_read_buffer / 50;

  float adc_volt = (float)(analog_read_buffer / ADC_MAX_READ) * ADC_REF;
  // debugf_info("batmes_adc_volt:%.4f\n", adc_volt);
  return adc_volt / BAT_VOLTAG_DIV;
}

/*returns the current consumption at the moment of the PCB in A*/
float get_current()
{
  const float R_SHUNT = 0.01; // in Ohms
  const float Gain = 100;     // in Ohms

  analogReadResolution(ADC_RES);
  pinMode(PIN_CURR, INPUT);

  float analog_read_buffer = 0;
  for (int i = 0; i < 50; i++)
  {
    analog_read_buffer += analogRead(PIN_CURR);
  }
  analog_read_buffer = analog_read_buffer / 50;

  float adc_volt = (float)(analog_read_buffer / ADC_MAX_READ) * ADC_REF;
  // debugf_info("batmes_adc_curr:%.4f\n", adc_volt);
  return adc_volt / (R_SHUNT * Gain);
}

/*print current Stack/Heap use*/
void printMemoryUse()
{
  debugf_info("-freeStack: %.2f kbytes\n", (float)rp2040.getFreeStack() * 0.001);
  debugf_info("-freeHeadp: %.2f kbytes | usedHeap: %.2f kbytes\n", (float)rp2040.getFreeHeap() * 0.001, (float)rp2040.getUsedHeap() * 0.001);
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
 *  | sd init | cableTest  | TCP init | Heater init | Oxy init | Light init | default 0  | Ki   |  Kd   |  Kp   |
 *        0         1          2           3             4           5          6 - 7      8-15   16-23  24-32
 */
uint32_t get_Status()
{
  uint32_t status = 0;

  // sd
  status |= ((uint32_t)sd_init << 0);

  // TCP connection
  status |= ((uint32_t)tcp_link_status() << 1);

  // TCP init
  status |= ((uint32_t)TCP_init << 2);

  // Heater
  status |= ((uint32_t)heat_init << 3);

  // Oxygen Sensors
  status |= ((uint32_t)oxy_serial_init << 4);

  // Light
  status |= ((uint32_t)light_init << 5);

  // Default 0 bits
  status |= ((uint32_t)0 << 6);
  status |= ((uint32_t)0 << 7);

  // PID constants
  status |= ((uint32_t)ki << 8);  // Ki
  status |= ((uint32_t)kd << 16); // Kd
  status |= ((uint32_t)kp << 24); // Kp

  return status;
}

/**
 * returns what on what port a peripheral is connected
 * @return first 8 bit are the NTCs, next 8 are the oxygen sensors and the next 8 are the heaters
 */
uint32_t check_peripherals()
{
  uint32_t results = 0;
  /* NTC probes */
  results |= (temp_isconnected(NTC_PROBE_0) << 0);
  results |= (temp_isconnected(NTC_PROBE_1) << 1);
  results |= (temp_isconnected(NTC_PROBE_2) << 2);
  results |= (temp_isconnected(NTC_PROBE_3) << 3);
  results |= (temp_isconnected(NTC_PROBE_4) << 4);
  results |= (temp_isconnected(NTC_PROBE_5) << 5);

  /* Oxygen sensors */
  results |= (oxy_isconnected(NTC_PROBE_0) << 8);
  results |= (oxy_isconnected(NTC_PROBE_1) << 9);
  results |= (oxy_isconnected(NTC_PROBE_2) << 10);
  results |= (oxy_isconnected(NTC_PROBE_3) << 11);
  results |= (oxy_isconnected(NTC_PROBE_4) << 12);
  results |= (oxy_isconnected(NTC_PROBE_5) << 13);

  /* Heating */

  /*turns all heater off*/
  float buff_heat[] = {0, 0, 0, 0, 0, 0, 0, 0};
  heat_updateall(buff_heat);

  float cur_alloff = get_current(); // current with all heater off

  for (uint8_t i = 0; i < 7; i++)
  {
    /*turns one heater on and messures the current*/
    buff_heat[i] = 100.0;
    heat_updateall(buff_heat);
    float cur_one = get_current() - cur_alloff;
    buff_heat[i] = 0.0;

    /* checks if current increased*/
    if (((HEAT_CURRENT - 0.100) < cur_one) && (cur_one < (HEAT_CURRENT + 0.100)))
    {
      results |= (1 << 16 + i);
      // debug(cur_one);
    }
  }

  debug("NTCs: ");
  for (int i = 0; i < 7; i++)
  {
    debugf_info("%u|",(results & 0xFF) >> i & 1);
  }

  debug("\nOxyg: ");
  for (int i = 0; i < 7; i++)
  {
    debugf_info("%u|",((results >> 8) & 0xFF) >> i & 1);
  }

  debug("\nHeat: ");
  for (int i = 0; i < 7; i++)
  {
    debugf_info("%u|",((results >> 16) & 0xFF) >> i & 1);
  }
  debugln();

  return results;
}