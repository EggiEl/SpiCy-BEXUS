/*-----------Header file for global defines, objekts, variables and functions-----------*/
#ifndef HEADER_H
#define HEADER_H

#include <Arduino.h>
/*--------Settings-----------------------*/
#define DEBUG_MODE 1    /*actrivates debug statements. 0=disable,1=Serial,2=TCP*/
#define COLOUR_SERIAL 1 /*activates/deactivates Serial.printing with color*/
#define USB_ENABLE 0    /*enables single drive USB functions*/
#define ADC_REF 3.0
#define ADC_RES 12 // ADC Resolution in Bit
#define ADC_MAX_READ (pow(2, ADC_RES) - 1)
const unsigned int ADC_MAX_WRITE = 100; //  Value where analogRrite = 100% duty cycle
const unsigned int ADC_FREQ_WRITE = 30000;

#define WATCHDOG_TIMEOUT 2000 // neds to be 8000ms max i think
#define CONNECTIONTIMEOUT 20  /*Conntection Timeout of the tcp client*/

extern unsigned long nMOTHERBOARD_BOOTUPS; // this number is stored in the flash and increses with every reset of th uC
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

    LAN_MISO = MISO_SPI0, // Alias for LAN MISO
    LAN_MOSI = MOSI_SPI0, // Alias for LAN MOSI
    LAN_SCK = SCK_SPI0,   // Alias for LAN SCK

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
    NTC_PROBE_4 = 8,      // S8 of MUX
    NTC_PROBE_5 = 7,      // S7 of MUX
    NTC_SMD = 5,          // S5 of MUX
    NTC_PROBE_10kfix = 6, // S6 of MUX

    PIN_OX_RX = 1, // Oxygen Sensor RX
    PIN_OX_TX = 0, // Oxygen Sensor TX

    STATLED = 23, // Status LED

    PIN_VOLT = A2, // Voltage Pin
    PIN_CURR = A1, // Current Pin

    // Connection of the Light Sensor
    PIN_LIGHT_SDA = SDA0, // I2C Data Line for Light Sensor
    PIN_LIGHT_SCL = SCL0  // I2C Clock Line for Light Sensor
} PIN_MAPPING;
// // 2 busses of the GPIO extentions
// #define SDA0 2
// #define SCL0 3
// #define SDA1 4
// #define SCL1 5
// // Lan and SD ic are connected both on SPI0 Bus
// #define CS_LAN 17
// #define CS_SD 16
// #define MISO_SPI0 20
// #define SCK_SPI0 18
// #define MOSI_SPI0 19
// #define LAN_MISO MISO_SPI0
// #define LAN_MOSI MOSI_SPI0
// #define LAN_SCK SCK_SPI0
// /*V4.0 pins :*/
// // #define MISO_SPI0 0
// // #define CS_SD 1
// // #define SCK_SPI0 2
// // #define MOSI_SPI0 3
// // #define LAN_MISO 8
// // #define CS_LAN 9
// // #define LAN_MOSI 11
// // #define LAN_SCK 10
// #define PIN_H0 8
// #define PIN_H1 9
// #define PIN_H2 10
// #define PIN_H3 11
// #define PIN_H4 12
// #define PIN_H5 13
// #define PIN_H6 14
// #define PIN_H7 15
// // the temp probes and the oxygen sensors are connected to the same multiplexerlines axa if temp 0 is selected so is oxy0
// #define PIN_PROBEMUX_0 26
// #define PIN_PROBEMUX_1 25
// #define PIN_PROBEMUX_2 24
// #define PIN_MUX_OXY_DISABLE 21
// #define PIN_TEMPADC A3
// #define nNTC 8
// #define NTC_PROBE_0 1      // S1 of MUX
// #define NTC_PROBE_1 2      // S2 of MUX
// #define NTC_PROBE_2 3      // S3 of MUX
// #define NTC_PROBE_3 4      // S4 of MUX
// #define NTC_PROBE_4 8      // S8 of MUX
// #define NTC_PROBE_5 7      // S7 of MUX
// #define NTC_SMD 5          // S5 of MUX
// #define NTC_PROBE_10kfix 6 // S6 of MUX
// #define PIN_OX_RX 1
// #define PIN_OX_TX 0
// #define PIN_OXY_ENABLE 21
// #define STATLED 23 // Status LED
// #define PIN_VOLT A2
// #define PIN_CURR A1
// // COnnection of the Light Sensor
// #define PIN_LIGHT_SDA SDA0 // blue
// #define PIN_LIGHT_SCL SCL0 // yellow

/**
 * contains all data of one readout form the oxygen sensors
 * */
struct OxygenReadout
{
    int32_t error = 0;
    int32_t dphi = 0;
    int32_t umolar = 0;
    int32_t mbar = 0;
    int32_t airSat = 0;
    int32_t tempSample = 0;
    int32_t tempCase = 0;
    int32_t signalIntensity = 0;
    int32_t ambientLight = 0;
    int32_t pressure = 0;
    int32_t resistorTemp = 0;
    int32_t percentOtwo = 0;
    unsigned long timestamp_mesurement = 0;
};

/**
 * packet used for downlink.
 * please use packet_create() and packet_destroy() for good memory management
 * */
struct packet
{                                     // struct_format L L 6L 6f 6f 6i i f 2i 80s
    unsigned int id = 0;              // each packet has a unique id
    unsigned int timestampPacket = 0; // in ms
    float power[2] = {0};             // battery voltage in mV and current consumption in mA

    struct OxygenReadout oxy_measure[6];
    float light[12] = {0.0f};

    /**temperature from thermistors:
     *0-5 NTC cable
     *6 NTC SMD
     *7 fix reference value
     *8 cpu temp*/
    float thermistor[9] = {0};
    float heaterPWM[6] = {0}; // power going to heating
    float pid[3] = {0};
};

struct packet *packet_create();
void packettochar(struct packet *data, char buffer[]);
void destroy_packet(struct packet *p);

/*state mashine*/
void nextState();
void select_probe_or_NTC(const uint8_t ProbeorNTC);

/* multithreading */
extern uint8_t flag_pause_core1;
extern uint8_t flag_core1_isrunning;
void pause_Core1();
void resume_Core1();

/*tcp_client*/
extern volatile char TCP_init;
void tcp_setup_client();
void tpc_testmanually(int nPackets = 1, unsigned int nTries = 5);
char tcp_send_packet(struct packet *packet);
char tcp_send_multible_packets(struct packet **packet_buff, unsigned int nPackets);
void tcp_check_command();
void tcp_print_info();
void tpc_send_error(unsigned char error);
unsigned char tcp_link_status();

/*sd*/
extern volatile char sd_init;
void sd_setup();
int sd_numpackets(const char filepath[]);
bool sd_writestruct(struct packet *s_out, const char filepath[]);
bool sd_readstruct(struct packet *data, const char filepath[], unsigned long position);
bool sd_printfile(const char filepath[]);
bool sd_writetofile(const char *buffer_text, const char *filename);

/*status*/
extern unsigned long nMOTHERBOARD_BOOTUPS; // keep tracks of how often the Motherboard did boot up
uint32_t get_Status();

/*debug console*/
void handleCommand(char buffer_comand, float param1, float param2, float param3, float param4);
float get_batvoltage();
float get_current();
void checkSerialInput();
void StatusLedBlink(uint8_t LED);
void free_ifnotnull(void *pointer);
void printMemoryUse();

/*i2c scan*/
void scan_wire();

/*Heating*/
const float HEAT_VOLTAGE = 5;                              // in V
const float HEAT_RESISTANCE = 10;                          // in Ohm
const float HEAT_CURRENT = HEAT_VOLTAGE / HEAT_RESISTANCE; // current of a single Heater in A

extern volatile char heat_init;
void heat_setup();
void heat_updateall(const float HeaterPWM[]);
void heat_updateone(const uint8_t PIN, const float duty);
void heat_testmanual();

/*Pid*/
extern volatile char pid_init;
extern float kp;
extern float ki;
extern float kd;
void pid_setup();
void pid_update_all();

/*Thermistors*/
#define nNTC 8 // Number of NTC probes
extern volatile char temp_init;
void temp_setup();
float temp_read_one(uint8_t NTC, uint8_t nTimes = 100);
void temp_read_all(float buffer[8]);
void temp_log(const char path[], uint8_t NTC_Probe, uint8_t NTC_Ambient, unsigned long t_nextmeas_ms);
uint8_t temp_isconnected(uint8_t NTC = 255);

/*Oxygen Sensors*/
#define COMMAND_LENGTH_MAX 100 // how long a command string can possibly be
#define RETURN_LENGTH_MAX 100  // how long a return string can possibly be
#define OXY_BAUD 19200
#define OXY_SERIAL_TIMEOUT 550

extern volatile char oxy_serial_init;
void oxy_serial_setup();
void oxy_console();
uint8_t oxy_read_all(struct OxygenReadout mesure_buffer[6]);
char *oxy_commandhandler(const char command[], uint8_t nReturn = COMMAND_LENGTH_MAX);
uint8_t oxy_isconnected(const uint8_t PROBE = 255);

/*light spectrometers*/
extern volatile char light_init;
void light_setup();
void light_read(float *buffer, bool with_flash = 0);

/*single File USB*/
extern volatile char singleFileUsb_init;
void usb_singlefile_setup();
void usb_singlefile_update();
void headerCSV();
void plug(uint32_t i);
void unplug(uint32_t i);
void deleteCSV(uint32_t i);
void singlefile_close();

/*error handeling*/
enum
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

extern const uint8_t ERROR_DESTINATION_NO_TCP;
extern const uint8_t ERROR_DESTINATION_NO_SD;
extern const uint8_t ERROR_DESTINATION_NO_TCP_SD;
void error_handler(const unsigned int ErrorCode, const uint8_t destination = 0);

#endif
