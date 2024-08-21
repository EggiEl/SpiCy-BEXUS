#include "header.h"
#include "debug_in_color.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

const unsigned long nGPIOS = 29;

uint32_t check_peripherals();

/**
 * Reads Serial Commands wich start with "/".
 * /? for help
 *disabled if DEBUG_MODE == 0
 */
void check_serial_input()
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

      handle_command(buffer_comand, param1, param2, param3, param4);
    }
  }
#endif
}

/**
 *
 */
void handle_command(char buffer_comand, float param1, float param2, float param3, float param4)
{
  switch (buffer_comand)
  {
  case '?':
  {
    debugf_status("<help>\n");
    debugf_info("/b|Returns Battery Voltage and current\n\
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
/t|returns temperature value. 0-6 for external probes, 7 for uC one, -1 for all of them\n\
/x|prints barometer temperature and pressure\n\
/o|starts console to talk to PyroSience FD-OEM Oxygen Module\n\
/a|reads out light spectrometers\n\
/q|shut down microcontroller\n\
/c|[kp] [ki] [imax] [SET_TEMP_DEFAULT] sets pi controller gain values. \n\
   Set SET_TEMP_DEFAULT = -1000000.0 to deactivate PI controller\
    and be able to controller heater manually\
/u|starts an over_the_air OTA update\n");
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
    debugf_sucess("Status: %i\n", get_status());
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
    print_memory_use();
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
  case 'q':
  {
    debugf_status("Shutting down microcontroller\n");
    pause_Core1();
    float buffer[20] = {0};
    heat_updateall(buffer);
    while (1)
    {
      rp2040.wdt_reset();
      check_serial_input();
    }
    break;
  }
  case 'c':
  {
    PI_CONTROLLER *pi = 0;

    /*select controller*/
    switch ((int)param1)
    {
    case -1:

      break;
    case 0:
      pi = &pi_probe0;
      break;

    case 1:
      pi = &pi_probe1;
      break;

    case 2:
      pi = &pi_probe2;
      break;

    case 3:
      pi = &pi_probe3;
      break;

    case 4:
      pi = &pi_probe4;
      break;

    case 5:
      pi = &pi_probe4;
      break;

    default:
      break;
    }

    if (param2 != -1)
    {
      pi->kp = param2;
    }

    if (param3 != -1)
    {
      pi->ki = param3;
    }

    if (param4 != -1)
    {
      pi->I_MAX = param4;
    }

    debugf_info("Set pi controller of probe:%u p:%.4f i:%.4f i_max:%.4f", pi->kp, pi->ki, pi->I_MAX);

    break;
  }
  case 'u':
  {
    debugf_status("Over the air update. Waits for Serverfile\n");
    WiFiClient client;
    HTTPUpdate.update(client, "192.168.0.2", 80, "/arduino.bin");
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
void status_led_blink(uint8_t LED)
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
      analogWrite(LED, ADC_MAX_WRITE / 4);
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
  float adc_average = 0;
  for (int i = 0; i < 100; i++)
  {
    adc_average += analogRead(PIN_VOLT);
  }
  adc_average = adc_average / 100;

  float adc_volt = (float)(adc_average / ADC_MAX_READ) * ADC_REF;
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

  /*saves 100 readings and calculates average*/
  float adc_average = 0;
  float buffer[100];

  for (int i = 0; i < 100; i++)
  {
    int measure = analogRead(PIN_CURR);
    buffer[i] = measure;
    adc_average += measure;
  }
  adc_average = adc_average / 100.0;

  /*replaces every reading wich dividates strongly from average with average*/
  /*calculates new average from this*/
  float new_average = 0;
  for (int i = 0; i < 100; i++)
  {
    if ((buffer[i] > (adc_average * 1.3)) || (buffer[i] < (adc_average * 0.7)))
    {
      new_average += adc_average;
    }
    else
    {
      new_average += buffer[i];
    }
  }
  new_average = new_average / 100.0;

  /*calculates current*/
  float adc_volt = (float)(new_average / ADC_MAX_READ) * ADC_REF;
  // debugf_info("batmes_adc_curr:%.4f\n", adc_average);
  return adc_volt / (R_SHUNT * Gain);
}

/*print current Stack/Heap use*/
void print_memory_use()
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
uint32_t get_status()
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
  // status |= ((uint32_t)kp << 8);  // Ki
  // status |= ((uint32_t)ki << 16); // Kd

  return status;
}

/**
 * returns what on what port a peripheral is connected
 * @return
 * first 8 bits are the NTCs,
 * next 8 bits are the oxygen sensors,
 * next 8 bits are the heaters
 * next bit the AS7262 sensor
 * next bit AS7263 sensor
 * next 4 bit the RJ45 cable
 * -> |unknown|conmected|not connected|error
 * next bit battery voltage above 25 and below 30
 */
uint32_t check_peripherals()
{
  debugf_status("<peithals_detection>\n");
  uint32_t results = 0;
  /* NTC probes */
  rp2040.wdt_reset();
  results |= (temp_isconnected(NTC_OR_OxY_0) << 0);
  results |= (temp_isconnected(NTC_OR_OxY_1) << 1);
  results |= (temp_isconnected(NTC_OR_OxY_2) << 2);
  results |= (temp_isconnected(NTC_OR_OxY_3) << 3);
  results |= (temp_isconnected(NTC_4) << 4);
  results |= (temp_isconnected(NTC_5) << 5);

  /* Oxygen sensors */
  rp2040.wdt_reset();
  Serial1.setTimeout(500);
  results |= (oxy_isconnected(NTC_OR_OxY_0) << 8);
  rp2040.wdt_reset();
  results |= (oxy_isconnected(NTC_OR_OxY_1) << 9);
  rp2040.wdt_reset();
  results |= (oxy_isconnected(NTC_OR_OxY_2) << 10);
  rp2040.wdt_reset();
  results |= (oxy_isconnected(NTC_OR_OxY_3) << 11);
  rp2040.wdt_reset();
  results |= (oxy_isconnected(OxY_4) << 12);
  rp2040.wdt_reset();
  results |= (oxy_isconnected(OxY_5) << 13);
  rp2040.wdt_reset();
  Serial1.setTimeout(TIMEOUT_OXY_SERIAL);

  /* Heating */
  pause_Core1();
  /*turns all heater off*/
  float buff_heat[8] = {0.0};
  heat_updateall(buff_heat);

  float cur_alloff = get_current(); // current with all heater off

  for (uint8_t i = 0; i < 8; i++)
  {
    /*turns one heater on and messures the current*/
    buff_heat[i] = 100.0;
    heat_updateall(buff_heat);
    delay(1);
    float cur_one = get_current() - cur_alloff;
    // debugln(cur_one);
    buff_heat[i] = 0.0;

    /* checks if current increased*/
    if ((HEAT_CURRENT * 0.5 < cur_one))
    {
      results |= (1 << 16 + i);
      // debug(cur_one);
    }
  }
  resume_Core1();

  debugf_info("!For heater mesasurement y need to connect the jumper!\n");
  debugf_info("     |0|1|2|3|4|5|6|7|\n");
  debugf_info("NTCs: ");
  for (int i = 0; i < 6; i++)
  {
    debugf_info("%u|", (results & 0xFF) >> i & 1);
  }

  debugf_info("\nOxyg: ");
  for (int i = 0; i < 6; i++)
  {
    debugf_info("%u|", ((results >> 8) & 0xFF) >> i & 1);
  }

  debugf_info("\nHeat: ");
  for (int i = 0; i < 8; i++)
  {
    debugf_info("%u|", ((results >> 16) & 0xFF) >> i & 1);
  }
  debugf_info("\n");

  uint8_t light_connection = light_connected();

  debugf_info("\nLight: ");
  switch (light_connection)
  {
  case 0:
    debugf_info("np light sensor connected");
    break;
  case 2:
    debugf_info("only AS7262 connected");
    results |= (1 << 23);
    break;
  case 3:
    debugf_info("only AS7263 connected");
    results |= (1 << 24);
    break;
  case 5:
    debugf_info("both AS7262 and AS7263 are connected");
    results |= (1 << 23);
    results |= (1 << 24);
    break;
  default:
    break;
  }

  uint8_t cabletest_buff = tcp_link_status();
  debugf_info("\nRJ45 cable: ");
  switch (cabletest_buff)
  {
  case 0:
    debugf_error("unknown");
    results |= (1 << 25);
    break;
  case 1:
    debugf_sucess("connected");
    results |= (1 << 26);
    break;
  case 2:
    debugf_error("not connected");
    results |= (1 << 27);
    break;
  default:
    debugf_error("error in code. Ethernet.linkStatus() = %i", cabletest_buff);
    results |= (1 << 28);
    break;
  }

  debugf_info("\n25.0V< V_Bat <30.0 ");
  if (25.0 < get_batvoltage() < 30.0)
  {
    debugf_sucess("connected");
    results |= (1 << 29);
  }
  else
  {
    debugf_error("not connected");
    results |= (1 << 29);
  }

  return results;
}

#include <SFE_BMP180.h>
#include <Wire.h>

void read_out_BMP180()
{
  static SFE_BMP180 pressure;
  // const float ALTITUDE = 1655.0; // Altitude of SparkFun's HQ in Boulder, CO. in meters
  static uint8_t init = 0;
  if (!init)
  {
    init = 1;
    Serial.begin(9600);
    Serial.println("REBOOT");

    // Initialize the sensor (it is important to get calibration values stored on the device).

    Wire.setSCL(SCL1);
    Wire.setSDA(SDA1);
    if (pressure.begin())
      Serial.println("BMP180 init success");
    else
    {
      // Oops, something went wrong, this is usually a connection problem,
      // see the comments at the top of this sketch for the proper connections.

      Serial.println("BMP180 init fail\n\n");
      init = 0; // Pause forever.
      return;
    }
  }

  char status;

  // If you want sea-level-compensated pressure, as used in weather reports,
  // you will need to know the altitude at which your measurements are taken.
  // We're using a constant called ALTITUDE in this sketch:

  // Serial.println();
  // Serial.print("provided altitude: ");
  // Serial.print(ALTITUDE, 0);
  // Serial.print(" meters, ");
  // Serial.print(ALTITUDE * 3.28084, 0);
  // Serial.println(" feet");

  // If you want to measure altitude, and not pressure, you will instead need
  // to provide a known baseline pressure. This is shown at the end of the sketch.
  // You must first get a temperature measurement to perform a pressure reading.

  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.
    double T, P;
    // double p0, a;
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      // Serial.print("temperature: ");
      // Serial.print(T, 2);
      // Serial.print(" deg C, ");
      // Serial.print((9.0 / 5.0) * T + 32.0, 2);
      // Serial.println(" deg F");

      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          // Print out the measurement:
          // Serial.print(">absolute pressure: ");
          // Serial.print(P, 2);
          // Serial.print(" mb\n");
          Serial.println(P, 2);

          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          // p0 = pressure.sealevel(P, ALTITUDE); // we're at 1655 meters (Boulder, CO)
          // Serial.print("relative (sea-level) pressure: ");
          // Serial.print(p0, 2);
          // Serial.print(" mb, ");
          // Serial.print(p0 * 0.0295333727, 2);
          // Serial.println(" inHg");

          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          // a = pressure.altitude(P, p0);
          // Serial.print("computed altitude: ");
          // Serial.print(a, 0);
          // Serial.print(" meters, ");
          // Serial.print(a * 3.28084, 0);
          // Serial.println(" feet");
        }
        else
          Serial.println("error retrieving pressure measurement\n");
      }
      else
        Serial.println("error starting pressure measurement\n");
    }
    else
      Serial.println("error retrieving temperature measurement\n");
  }
  else
    Serial.println("error starting temperature measurement\n");
}