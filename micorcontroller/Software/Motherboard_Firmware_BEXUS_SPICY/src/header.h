/*-----------Header file for global defines, objekts, variables and functions-----------*/

#ifndef HEADER_H
#define HEADER_H

#include <Arduino.h>
#include "debug_in_color.h"
/*--------Settings-----------------------*/
#define DEBUG 1         /*actrivates debug statements. 0=disable,1=Serial,2=TCP*/
#define COLOUR_SERIAL 1 /*activates/deactivates Serial.printing with color*/
#define USB_ENABLE 0    /*enables single drive USB functions*/
#define ADC_RES 12      // ADC Resolution in Bit
#define ADC_MAX pow(2, ADC_RES)
/*----------------Pin mapping-------------*/
#if 1 // if statement to make code colapsable
#define MISO_SD 0
#define CS_SD 1
#define SCK_SD 2
#define MOSI_SD 3

#define MISO_LAN 8
#define CS_LAN 9
#define SCK_LAN 10
#define MOSI_LAN 11

// #define MISO_LAN 16
// #define CS_LAN 17
// #define SCK_LAN 18
// #define MOSI_LAN 19

#define PIN_H0u1 22
#define PIN_H2u3 23
#define PIN_H4 18
#define PIN_H5 19
#define PIN_H6 20
#define PIN_H7 21

#define PIN_TEMPADC A3
#define PIN_TEMPMUX_0 24
#define PIN_TEMPMUX_1 25
#define PIN_TEMPMUX_2 26
#define NTC_0 1      // S1 of MUX
#define NTC_1 2      // S2 of MUX
#define NTC_2 3      // S3 of MUX
#define NTC_3 4      // S4 of MUX
#define NTC_4 8      // S8 of MUX
#define NTC_5 7      // S7 of MUX
#define NTC_SMD 5    // S5 of MUX
#define NTC_10kfix 6 // S6 of MUX

#define STATLED_R 13 // Status LeD pins
#define STATLED_G 12
#define STATLED_B 7

#define PIN_VOLT A2
#define PIN_CURR A1
#endif

/*struct_packet*/
struct packet
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

struct packet *packet_create();
char *packettochar(struct packet *data);
void packet_print(struct packet *pkt);
void packet_writeinfo(struct packet *data, const char *info);

/*i2c scan*/
void scan_wire();

/*tcp_client*/
extern char TCP_init;
void tpc_testmanually(int nPackets = 1, unsigned int nTries = 5);
void tcp_setup_client();
char tcp_send_packet(struct packet *data);
char tcp_send_multible_packets(struct packet **data, unsigned int nPackets);
void tcp_check_command();
void tcp_print_info();

/*sd*/
extern char sd_init;
void sd_setup();
int sd_numpackets(const char filepath[]);
bool sd_writestruct(struct packet *s_out, const char filepath[]);
bool sd_readstruct(struct packet *data, const char filepath[], unsigned long position);
bool sd_printfile(const char filepath[]);

/*Heating*/
const unsigned int HEAT_FREQ = 100;           // 5kmax for the facy Ics
const unsigned int HEAT_HUNDERTPERCENT = 100; // value where the heaters are fully turned on
const unsigned int HEAT_CURRENT = 317;        // aproximation of the current of a single Heater in mA

extern char heat_init;
void heat_setup();
void heat_updateall(uint16_t *HeaterPWM);
void heat_updateone(uint8_t PIN, uint16_t PWM);
void heat_testmanual();
uint8_t heat_testauto();

/*Sensors*/
float temp_read_one(uint8_t Number);
float *temp_read_cable();

/*status*/
uint32_t get_Status();

/*utilitly functions*/
void handleCommand(char buffer_comand, float param1, float param2, float param3, float param4);
float get_batvoltage();
float get_current();
void checkSerialInput();
void StatusLedBlink(uint8_t LED);
void StatusLed_Downlink();

/*single File USB*/
extern char singleFileUsb_init;
void usb_singlefile_setup();
void usb_singlefile_update();
void headerCSV();
void plug(uint32_t i);
void unplug(uint32_t i);
void deleteCSV(uint32_t i);
void singlefile_close();

/*Pressure Sensor*/
void pressure_setup();
float pressure_read();
void pressure_example();

/*Oxygen Sensors*/
char oxy_commandhandler(uint8_t nParam, const char * Command, uint8_t channel, char param0 = 0, char param1 = 0, char param2 = 0, char param3 = 0) ;
float *oxy_readall();

/*state mashine*/
void nextState();
#endif
