/*-----------Header file for global defines, objekts, variables and functions-----------*/

#ifndef HEADER_H
#define HEADER_H

#include <Arduino.h>
#include "debug_in_color.h"

/*--------Settings-----------------------*/
#define USB_ENABLE 1 /*enables single drive USB functions*/

/*----------------Pin mapping-------------*/
#if 1 // if statement to make code colapsable
// #define MISO_LAN 0
// #define CS_LAN 1
// #define SCK_LAN 2
// #define MOSI_LAN 3

#define MISO_LAN 16
#define CS_LAN 17
#define SCK_LAN 18
#define MOSI_LAN 19

#define MISO_SD 0
#define MOSI_SD 3
#define CS_SD 1
#define SCK_SD 2

#define PIN_H0 6
#define PIN_H1 7
#define PIN_H2 8
#define PIN_H3 9
#define PIN_H4 10
#define PIN_H5 11
#define PIN_H6 12
#define PIN_H7 13

#define STATLED_R 23 // Status LeD pins
#define STATLED_G 22
#define STATLED_B 21

#define PIN_VOLT A3
#define PIN_CURR A2
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
void test_TCP_manually(int nPackets = 1, unsigned int nTries = 5);
void setup_TCP_Client();
char send_TCP_packet(struct packet *data);
char send_multible_TCP_packet(struct packet **data, unsigned int nPackets);
void recieve_TCP_command();
void TCP_print_info();
uint8_t cabletest();
void testServer();

/*sd*/
extern char SD_init;
void init_SD();
int sd_numpackets(const char filepath[]);
bool writestruct(struct packet s_out, const char filepath[]);
bool readstruct(struct packet *data, const char filepath[], unsigned long position);
bool printfile(const char filepath[]);

/*thermal*/
const unsigned int HEAT_FREQ = 1000;          // 5kmax for the facy Ics
const unsigned int HEAT_HUNDERTPERCENT = 100; // value where the heaters are fully turned on
const unsigned int HEAT_CURRENT = 317;        // aproximation of the current of a single Heater in mA

extern char heat_init;
void heat_setup();
void heat_updateall(uint16_t *HeaterPWM);
void heat_updateone(uint8_t PIN, uint16_t PWM);
void heat_testmanual();
uint8_t heat_testauto();

/*status*/
uint32_t get_Status();

/*utilitly functions*/
void handleCommand(char buffer_comand, float param1, float param2, float param3, float param4);
unsigned long get_batvoltage();
unsigned long get_current();
void checkSerialInput();
void StatusLedBlink();
void blinkLed(uint8_t PIN);
void fadeLED(uint8_t PIN);
void heartbeat();
void printMemoryUse();
unsigned int free_Ram_left();

/*single File USB*/
extern char singleFileUsb_init;
void usb_singlefile_setup();
void usb_singlefile_update();
void headerCSV();
void plug(uint32_t i);
void unplug(uint32_t i);
void deleteCSV(uint32_t i);
void singlefile_close();

#endif
