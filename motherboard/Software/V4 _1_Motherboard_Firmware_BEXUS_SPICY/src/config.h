/*--------config file----------------*/
#ifndef CONFIG_H
#define CONFIG_H

#include "Arduino.h"

/*Debug*/
#define DEBUG_MODE 1 /**activates debug statements.                      \
0 = disable                                                              \
1 = Serial                                                               \
2 = TCP -> dosn´t print error message but instead downlinks error codes \
3 = Serial & TCP*/

#define DEBUG_LEVEL 3 /*changes the debug console prints. \
0 = just errors                                           \
1 = Status Updates and sucess                             \
2 = Infos about running code                              \
3 = Debug infos                                           \
4 = state infos*/

#define DEBUG_COLOUR 1 /*activates/deactivates colour in debug outputs. \
Can decrease bandwith when colors disabled.*/

/*ADC/Analog Write*/
const float ADC_REF = 3.0;                               // reference voltage of the adc converter
const unsigned int ADC_RES = 12;                         // adc resolution in bit
const unsigned int ADC_MAX_READ = (pow(2, ADC_RES) - 1); // values adc generates when adc_ref voltage is applied to adc
const unsigned int ADC_MAX_WRITE = 100;                  // value where analogRrite = 100% duty cycle
const unsigned int ADC_FREQ_WRITE = 30000;               // frequency of adc write. therefore of the heater

/*Timeouts*/
const unsigned long TIMEOUT_WATCHDOG = 4000;     // in ms. 8000ms maximal
const unsigned long TIMEOUT_TCP_CONNECTION = 20; // conntection timeout of the tcp client
const unsigned long TIMEOUT_LIGHT_SENSOR = 100;  // timeout i2c connection light sensor
const unsigned long TIMEOUT_OXY_SERIAL = 300;    // timeout usart oxygen

/*Heating*/
const float HEAT_VOLTAGE = 7.7459666;                                   // in V
const float HEAT_RESISTANCE = 12;                                       // in Ohm
const float HEAT_CURRENT = HEAT_VOLTAGE / HEAT_RESISTANCE;              // current of a single Heater in A
const float HEAT_POWER = HEAT_CURRENT * HEAT_CURRENT * HEAT_RESISTANCE; // max heating power of a single heater
const float PID_MAX = HEAT_POWER;                                       // max PID value. is normed on heating_power

/*FD-OEM Oxygen sensor*/
const unsigned long COMMAND_LENGTH_MAX = 100; // how long a command string can possibly be
const unsigned long RETURN_LENGTH_MAX = 100;  // how long a return string can possibly be
const unsigned long OXY_BAUD = 19200;         // baud of oxygen usart connection

/*PI controller*/
const unsigned long PI_T = 1000;                      // time in ms till next PID controller update. (1/Frequency of PI Controller)
const float SET_TEMP_DEFAULT = 34.0;                  // temperature target for PI controller. Set to -1000000.0 to disable Controller
const unsigned long PI_SWEEP_PRINT_DELAY = 60 * 1000; // delay between printout of PI_SWEEP

/*light sensors*/
#include "Adafruit_LTR390.h"
const ltr390_gain_t LIGHT_LTR390_GAIN = LTR390_GAIN_3;
const float LIGHT_LTR390_WFAC = 1;
const float LIGHT_LTR390_INT = 4;
const float LIGHT_LTR390_UV_SENSITIFITY = 2300;
/*Pin mapping*/
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
    PIN_MUX_ADRES_0 = 26,     // Multiplexer Select Line 0
    PIN_MUX_ADRES_1 = 25,     // Multiplexer Select Line 1
    PIN_MUX_ADRES_2 = 24,     // Multiplexer Select Line 2
    PIN_MUX_OXY_DISABLE = 21, // Multiplexer Oxygen Disable

    PIN_TEMPADC = A3, // Analog-to-Digital Converter for temperature

    NTC_OR_OxY_0 = 1, // S1 of MUX
    NTC_OR_OxY_1 = 2, // S2 of MUX
    NTC_OR_OxY_2 = 3, // S3 of MUX
    NTC_OR_OxY_3 = 4, // S4 of MUX
    NTC_4 = 8,        // S8 of MUX
    NTC_5 = 7,        // S7 of MUX
    NTC_SMD = 5,      // S5 of MUX
    NTC_10kfix = 6,   // S6 of MUX

    OxY_4 = 5,
    OxY_5 = 6,

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

/*errors*/
enum ERRORS
{
    ERROR_SD_INI,
    ERROR_SD_COUNT,
    ERROR_WR_STR,
    ERROR_SD_WRITE_OPEN,
    ERROR_SD_PINMAP,

    ERROR_TCP_INI,
    ERROR_TCP_COMMAND_PARSING,
    ERROR_TCP_PARAM_CORRUPT,
    ERROR_TCP_COMMAND_CORRUPT,
    ERROR_TCP_SEND_FAILED,
    ERROR_TCP_SEND_TIMEOUT,
    ERROR_TCP_SERVER_INVALID,
    ERROR_TCP_TRUNCATED,
    ERROR_TCP_TRUNCATED_2,
    ERROR_TCP_CABLE_DISCO,
    ERROR_TCP_NO_RESPONSE,
    ERROR_TCP_DEBUG_MEMORY,
    ERROR_TCP_CLIENT_CONNTECION,
    ERROR_TCP_SEND_MULTIBLE_FAILED,

    ERROR_HEAT_INI,

    ERROR_LIGHT_INI,

    ERROR_NO_NTC_CONNECTED,

    ERROR_STATE,

    ERROR_PACK_ID_OV,
    ERROR_PACK_MEM_AL,
    ERROR_PACKAGE_FREE_TWICE,

    ERROR_OXY_INI,
    ERROR_OXY_AUTO_AMP,
    ERROR_OXY_SIGNAL_INT_LOW,
    ERROR_OXY_OPTICAL_DETECTOR_SATURATED,
    ERROR_OXY_REF_SIGNAL_LOW,
    ERROR_OXY_REF_SIGNAL_HIGH,
    ERROR_OXY_SAMPLE_TEMP_SENSOR,
    ERROR_OXY_RESERVED,
    ERROR_OXY_HIGH_HUMIDITY,
    ERROR_OXY_CASE_TEMP_SENSOR,
    ERROR_OXY_PRESSURE_SENSOR,
    ERROR_OXY_HUMIDITY_SENSOR
};

/*parometer testing*/
#define BAROMETER_TEATING 0
#endif