#ifndef CONFIG_H
#define CONFIG_H

#include "Arduino.h"

/*-------Debug-----------------------*/
#define DEBUG_MODE 0    /*actrivates debug statements. 0=disable,1=Serial,2=TCP*/
#define COLOUR_SERIAL 1 /*activates/deactivates Serial.printing with color*/

/*------- ADC/Analog Write------------*/
#define ADC_REF 3.0
#define ADC_RES 12 // ADC Resolution in Bit
#define ADC_MAX_READ (pow(2, ADC_RES) - 1)
const unsigned int ADC_MAX_WRITE = 100; //  Value where analogRrite = 100% duty cycle
const unsigned int ADC_FREQ_WRITE = 30000;

/*------Timeouts.......................*/
const unsigned long TIMEOUT_WATCHDOG = 4000;     // neds to be 8000ms max i think
const unsigned long TIMEOUT_TCP_CONNECTION = 20; /*Conntection Timeout of the tcp client*/
const unsigned long TIMEOUT_LIGHT_SENSOR = 100;
const unsigned long TIMEOUT_OXY_SERIAL = 300;

/*------Heating-------------------------*/
const float HEAT_VOLTAGE = 5;                              // in V
const float HEAT_RESISTANCE = 12;                          // in Ohm
const float HEAT_CURRENT = HEAT_VOLTAGE / HEAT_RESISTANCE; // current of a single Heater in A
const float HEAT_POWER = HEAT_CURRENT * HEAT_CURRENT * HEAT_RESISTANCE;
const float PID_MAX = HEAT_POWER;

/*------Oxygen-----------------*/
#define COMMAND_LENGTH_MAX 100 // how long a command string can possibly be
#define RETURN_LENGTH_MAX 100  // how long a return string can possibly be
#define OXY_BAUD 19200

/*------PI controller--------------------*/
const unsigned long PI_T = 1000;     // time in ms till next PID controller update. (1/Frequency of PI Controller)
const float SET_TEMP_DEFAULT = 34.0; // temperature target for PI controller. Set to -1000000.0 to disable Controller
const unsigned long PI_SWEEP_PRINT_DELAY = 60 * 1000; //delay between printout of PI_SWEEP
/*----------------Pin mapping-------------*/
typedef enum
{
    // GPIO extension buses
    SDA0 = 2, // I2C Data Line 0
    SCL0 = 3, // I2C Clock Line 0

    SDA1 = 4, // I2C Data Line 1
    SCL1 = 5, // I2C Clock Line 1

    // LAN and SD card connected on SPI0 Bus
    CS_LAN = 17,    // Chip Select for LAN
    CS_SD = 16,     // Chip Select for SD Card
    MISO_SPI0 = 20, // Master In Slave Out for SPI0
    SCK_SPI0 = 18,  // Serial Clock for SPI0
    MOSI_SPI0 = 19, // Master Out Slave In for SPI0

    // LAN_MISO = MISO_SPI0, // Alias for LAN MISO
    // LAN_MOSI = MOSI_SPI0, // Alias for LAN MOSI
    // LAN_SCK = SCK_SPI0,   // Alias for LAN SCK

    // V4.0 pins (commented out)
    // MISO_SPI0 = 0,
    // CS_SD = 1,
    // SCK_SPI0 = 2,
    // MOSI_SPI0 = 3,
    // LAN_MISO = 8,
    // CS_LAN = 9,
    // LAN_MOSI = 11,
    // LAN_SCK = 10,

    // High pins
    PIN_H0 = 8,
    PIN_H1 = 9,
    PIN_H2 = 10,
    PIN_H3 = 11,
    PIN_H4 = 12,
    PIN_H5 = 13,
    PIN_H6 = 14,
    PIN_H7 = 15,

    // Multiplexer lines for temperature probes and oxygen sensors
    PIN_PROBEMUX_0 = 26,      // Multiplexer Select Line 0
    PIN_PROBEMUX_1 = 25,      // Multiplexer Select Line 1
    PIN_PROBEMUX_2 = 24,      // Multiplexer Select Line 2
    PIN_MUX_OXY_DISABLE = 21, // Multiplexer Oxygen Disable

    PIN_TEMPADC = A3, // Analog-to-Digital Converter for temperature

    NTC_PROBE_0 = 1,      // S1 of MUX
    NTC_PROBE_1 = 2,      // S2 of MUX
    NTC_PROBE_2 = 3,      // S3 of MUX
    NTC_PROBE_3 = 4,      // S4 of MUX
    NTC_4 = 8,            // S8 of MUX
    NTC_5 = 7,            // S7 of MUX
    NTC_SMD = 5,          // S5 of MUX
    NTC_PROBE_10kfix = 6, // S6 of MUX

    PROBE_4 = 5,
    PROBE_5 = 6,

    PIN_OX_RX = 0, // Oxygen Sensor RX
    PIN_OX_TX = 1, // Oxygen Sensor TX

    STATLED = 23, // Status LED

    PIN_VOLT = A2, // Voltage Pin
    PIN_CURR = A1, // Current Pin

    // Connection of the Light Sensor
    PIN_LIGHT_SDA_0 = SDA0, // I2C Data Line for Light Sensor AS7262
    PIN_LIGHT_SCL_0 = SCL0, // I2C Clock Line for Light Sensor AS7262

    PIN_LIGHT_SDA_1 = SDA1, // I2C Data Line for Light Sensor AS7263
    PIN_LIGHT_SCL_1 = SCL1  // I2C Clock Line for Light Sensor AS7263
} PIN_MAPPING;

#endif