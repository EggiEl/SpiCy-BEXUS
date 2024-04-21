#ifndef HEADER_H
#define HEADER_H

#include <Arduino.h>

#define DEBUG 1
#define USB_ENABLE 1
#define MICROS 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)

#if MICROS == 1
#define MESSURETIME_START        \
    unsigned long ta = micros(); \
    unsigned long tb;
#define MESSURETIME_STOP \
    debug("-Time:");     \
    tb = micros() - ta;  \
    debug(tb);           \
    debug("us");
#else
#define MESSURETIME_START        \
    unsigned long ta = millis(); \
    unsigned long tb;
#define MESSURETIME_STOP \
    debug("-Time:");     \
    tb = millis() - ta;  \
    debug(tb);           \
    debug("ms");
#endif

#else
#define debug(x)
#define debugln(x)
#define MESSURETIME_START
#define MESSURETIME_STOP
#endif

/*----------------Pin mapping-------------*/
#define MISO_LAN 0 // 16
#define CS_LAN 1   // 17
#define SCK_LAN 2  // 18
#define MOSI_LAN 3 // 19

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

/*struct_packet*/
extern volatile unsigned long id_struct;
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
struct packet packet_create();
char *packettochar(struct packet data);
void packet_print(struct packet pkt);
void packet_writeinfo(struct packet &data, const char *info);

/*i2c scan*/
void scan_wire();

/*tcp_client*/
extern char TCP_init;
void test_TCP_manually();
void setup_TCP_Client();
void send_TCP(char *data, unsigned long int size);
void recieve_TCP_command();
int send_TCP_packet(struct packet data);
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
#define HEAT_FREQ 1000          // 5kmax for the facy Ics
#define HEAT_HUNDERTPERCENT 100 // value where the heaters are fully turned on
#define HEAT_CURRENT 317        // aproximation of the current of a single Heater in mA

extern char heat_init;
void heat_initialize();
void heat_updateall(uint16_t *HeaterPWM);
void heat_updateone(uint8_t PIN, uint16_t PWM);
void heat_testmanual();
uint8_t heat_testauto();

/*status*/
uint32_t get_Status();

/*utilitly functions*/
unsigned long get_batvoltage();
unsigned long get_current();
void serial_commands();
void StatusLedBlink();
void blinkLed();
void fadeLED();
void heartbeat();

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