/*-----------Header file for global defines, objekts, variables and functions-----------*/
#ifndef HEADER_H
#define HEADER_H

#include <Arduino.h>
#include "debug_in_color.h"
/*--------Settings-----------------------*/
#define DEBUG 1         /*actrivates debug statements. 0=disable,1=Serial,2=TCP*/
#define COLOUR_SERIAL 1 /*activates/deactivates Serial.printing with color*/
#define USB_ENABLE 0    /*enables single drive USB functions*/
#define ADC_REF 3.0
#define ADC_RES 12 // ADC Resolution in Bit
#define ADC_MAX_READ (pow(2, ADC_RES) - 1)

#define WATCHDOG_TIMEOUT 8000 // neds to be 8000ms max i think
#define CONNECTIONTIMEOUT 20  /*Conntection Timeout of the tcp client*/

const unsigned int ADC_MAX_WRITE = 100; //  Value where analogRrite = 100% duty cycle
const unsigned int ADC_FREQ_WRITE = 300;
/*----------------Pin mapping-------------*/
#if 1 // if statement to make code colapsable

// 2 busses of the GPIO extentions
#define SDA0 2
#define SCL0 3

#define SDA1 4
#define SCL1 5

// Lan and SD ic are connected both on SPI0 Bus
// #define CS_LAN 17
// #define CS_SD 16
// #define MISO_SPI0 20
// #define SCK_SPI0 18
// #define MOSI_SPI0 19

// #define LAN_MISO MISO_SPI0
// #define LAN_MOSI MOSI_SPI0
// #define LAN_SCK SCK_SPI0

/*V4.0 pins :*/
#define MISO_SPI0 0
#define CS_SD 1
#define SCK_SPI0 2
#define MOSI_SPI0 3
#define LAN_MISO 8
#define CS_LAN 9
#define LAN_MOSI 11
#define LAN_SCK 10

#define PIN_H0 8
#define PIN_H1 9
#define PIN_H2 10
#define PIN_H3 11
#define PIN_H4 12
#define PIN_H5 13
#define PIN_H6 14
#define PIN_H7 15

// the temp probes and the oxygen sensors are connected to the same multiplexerlines axa if temp 0 is selected so is oxy0
#define PIN_PROBEMUX_0 26
#define PIN_PROBEMUX_1 25
#define PIN_PROBEMUX_2 24

#define PIN_TEMPADC A3
#define nNTC 8
#define NTC_0 1      // S1 of MUX
#define NTC_1 2      // S2 of MUX
#define NTC_2 3      // S3 of MUX
#define NTC_3 4      // S4 of MUX
#define NTC_4 8      // S8 of MUX
#define NTC_5 7      // S7 of MUX
#define NTC_SMD 5    // S5 of MUX
#define NTC_10kfix 6 // S6 of MUX

#define PIN_OX_RX 1
#define PIN_OX_TX 0
#define PIN_OXY_ENABLE 21

#define STATLED 23 // Status LED

#define PIN_VOLT A2
#define PIN_CURR A1

// COnnection of the Light Sensor
#define PIN_LIGHT_SDA SDA0 // blue
#define PIN_LIGHT_SCL SCL0 // yellow
#endif

/*struct_packet*/
struct packetold
{ // struct_format L L 6L 6f 6f 6i i f 2i 80s
    unsigned long id = 0;
    unsigned long timestampPacket = 0;

    unsigned long timestampOxy[6] = {0};

    float oxigen[6] = {0.0f};
    float tempTube[6] = {0.0f};

    int heaterPWM[6] = {0};
    int error = 0;

    float tempCpu = analogReadTemp(3.3f);
    unsigned long power[2] = {0};

    char info[80] = {0};
};

/**
 * packet used for downlink.
 * please use packet_create() and packet_destroy() for good memory management
 * */
struct packet                         // 2L 2f 6L 6I 6I 6I 8I 6I 18f 20s
{                                     // struct_format L L 6L 6f 6f 6i i f 2i 80s
    unsigned int id = 0;              // each packet has a unique id
    unsigned int timestampPacket = 0; // in ms
    float power[2] = {0};             // battery voltage in mV and current consumption in mA

    unsigned int pyro_timestamp[6] = {0}; //
    unsigned int pyro_temp[6] = {0};      // temp on the sensor pcb in °C * 100
    unsigned int pyro_oxy[6] = {0};       // Oxygenvalue
    unsigned int pyro_pressure[6] = {0};  // pressure?
    float light[12] = {0.0f};

    /**temperature from thermistors:
     *0-5 NTC cable
     *6 Nlog regelnTC SMD
     *7 fix reference value
     *8 cpu temp*/
    unsigned int thermistor[9] = {0};
    unsigned int heaterPWM[6] = {0}; // power going to heating
    float pid[6 * 3] = {0};
};

struct packet *packet_create();
char *packettochar(struct packet *data);
void packet_print(struct packetold *pkt);
void destroy_packet(struct packet *p);

/*state mashine*/
void nextState();

/*tcp_client*/
extern char TCP_init;
void tcp_setup_client();
void tpc_testmanually(int nPackets = 1, unsigned int nTries = 5);
char tcp_send_packet(struct packet *data);
char tcp_send_multible_packets(struct packet **data, unsigned int nPackets);
void tcp_check_command();
void tcp_print_info();
void tpc_send_error(unsigned char error);

/*sd*/
extern char sd_init;
void sd_setup();
int sd_numpackets(const char filepath[]);
bool sd_writestruct(struct packet *s_out, const char filepath[]);
bool sd_readstruct(struct packet *data, const char filepath[], unsigned long position);
bool sd_printfile(const char filepath[]);
bool sd_writetofile(const char *buffer_text, const char *filename);

/*status*/
uint32_t get_Status();

/*debug console*/
void handleCommand(char buffer_comand, float param1, float param2, float param3, float param4);
float get_batvoltage();
float get_current();
void checkSerialInput();
void StatusLedBlink(uint8_t LED);
void free_ifnotnull(void *pointer);

/*i2c scan*/
void scan_wire();

/*Heating*/
const float HEAT_VOLTAGE = 30.01;                                                           // in V
const float HEAT_RESISTANCE = 10;                                                           // in Ohm
const unsigned int HEAT_CURRENT = (uint)(((float)HEAT_VOLTAGE * 1000.0 / HEAT_RESISTANCE)); // current of a single Heater in mA

extern char heat_init;
void heat_setup();
void heat_updateall(uint16_t *HeaterPWM);
void heat_updateone(uint8_t PIN, uint16_t PWM);
void heat_testmanual();
uint8_t heat_testauto();

/*Pid*/
extern char pid_init;
void pid_setup();
void pid_update_all();

/*Thermistors*/
extern char temp_init;
void temp_setup();
float temp_read_one(uint8_t Number);
void temp_read_all(float *buffer);
void temp_print_ntc(uint8_t Pin);

/*Oxygen Sensors*/
#define COMMAND_LENGTH_MAX 100 // how long a command string can possibly be
#define RETURN_LENGTH_MAX 100  // how long a return string can possibly be
#define OXY_BAUD 19200
extern char oxy_init;
void oxy_setup(const uint8_t rx = PIN_OX_RX, const uint8_t tx = PIN_OX_TX);
void oxy_console();
bool oxy_read_all(float *buffer);
char *oxy_commandhandler(const char command[], uint8_t nReturn = COMMAND_LENGTH_MAX);

/*light spectrometers*/
extern char light_init;
void light_setup();
void light_read(float *buffer, bool with_flash = 0);

/*single File USB*/
extern char singleFileUsb_init;
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
    ERROR_SD_INI
};

void error_handler(unsigned int ErrorCode);

#endif
