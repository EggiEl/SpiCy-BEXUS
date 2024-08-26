/*-----------Header file for global defines, objekts, variables and functions-----------*/
#ifndef HEADER_H
#define HEADER_H

#include "config.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

/*packet management*/
// contains all data of one readout form the oxygen sensors
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
    int32_t humidity = 0;
    int32_t resistorTemp = 0;
    int32_t percentOtwo = 0;
    unsigned long timestamp_mesurement = 0;
};

// packet used for downlink.please use packet_create() and packet_destroy() for good memory management
struct packet
{                                     // struct_format L L 6L 6f 6f 6i i f 2i 80s
    unsigned int id = 0;              // each packet has a unique id
    unsigned int timestampPacket = 0; // in ms
    float power[2] = {0};             // battery voltage in mV and current consumption in mA

    struct OxygenReadout oxy_measure[6];
    float light[12] = {0};

    /**temperature from thermistors:
     *0-5 NTC cable
     *6 NTC SMD
     *7 fix reference value
     *8 cpu temp*/
    float thermistor[9] = {0};
    float heaterPWM[6] = {0}; // power going to heating
    float pid[3] = {0};       // kp and ki
};

struct packet *packet_create();
void packettochar(struct packet *data, char buffer[]);
void destroy_packet(struct packet *p);

/*state mashine*/
void next_state();
void select_oxy_or_ntc(const int ProbeorNTC);

/*multithreading*/
extern uint8_t flag_pause_core1;
extern uint8_t flag_core1_isrunning;
void pause_Core1();
void resume_Core1();

/*tcp_client*/
extern volatile char TCP_init;
void tcp_setup_client();
void tpc_testmanually(int nPackets = 1, unsigned int nTries = 5);
char tcp_send_packet(struct packet *packet);
void tcp_send_multible_packets(struct packet **packet_buff, unsigned int nPackets);
void tcp_check_command();
void tcp_print_info();
void tpc_send_error(const unsigned char error);
void tpc_send_string(const char string[]);
void tcp_sendf(const char *__restrict format, ...);
unsigned char tcp_link_status();
void tcp_receive_OTA_update(int size_firmware);

/*sd*/
extern volatile char sd_init;
void sd_setup();
int sd_numpackets(const char filepath[]);
bool sd_writestruct(struct packet *s_out, const char filepath[]);
bool sd_readstruct(struct packet *data, const char filepath[], unsigned long position);
bool sd_printfile(const char filepath[]);
bool sd_writetofile(const char *buffer_text, const char *filename);

/*status*/
uint32_t get_status();

/*debug console*/
extern unsigned long nMOTHERBOARD_BOOTUPS; // this number is stored in the flash and increses with every reset of th uC
void handle_command(char buffer_comand, float param1, float param2, float param3, float param4);
float get_batvoltage();
float get_current();
void check_serial_input();
void status_led_blink(uint8_t LED);
void print_memory_use();
uint32_t check_peripherals();
void read_out_BMP180();

/*i2c scan*/
void scan_wire();

/*Heating*/
extern volatile char heat_init;
void heat_setup();
void heat_updateall(const float HeaterPWM[8]);
void heat_updateone(const uint8_t PIN, const float duty);
void heat_testmanual();

/*Pid*/
typedef struct
{
    /*semi constant values*/
    float desired_temp = SET_TEMP_DEFAULT;
    uint8_t heater_pin = 0;
    uint8_t thermistor_pin = 0;
    float kp = 0;
    float ki = 0;
    float I_MAX = 0;

    /*static values*/
    float i_last = 0;
    unsigned long time_last = millis();
    float error_last = 0;
    float pi_last = 0;
    float heat = -1;
} PI_CONTROLLER;

extern PI_CONTROLLER pi_probe0;
extern PI_CONTROLLER pi_probe1;
extern PI_CONTROLLER pi_probe2;
extern PI_CONTROLLER pi_probe3;
extern PI_CONTROLLER pi_probe4;
extern PI_CONTROLLER pi_probe5;

void pi_update_all();
void pi_print_controller(PI_CONTROLLER *pi);
uint8_t pi_record_step_function(uint8_t PIN_HEATER, uint8_t PIN_NTC, float T_START, float TIME_TILL_STOP);

typedef struct
{
    /* Control parameters */
    float TEMP_COOL = 31;                                  // start tenperature for the PI controller to see an evtl. overshoot. [°C]
    float TEMP_SET = 32;                                   // Target temperature for the PI controller. [°C]
    unsigned long TIME_TILL_STOP = 0.5 * (60 * 60 * 1000); // testing/recording duration for a single cotroller. [ms]
    unsigned int nCYCLES = 10;                             // amount of cycles. Should be <= than elements in the gain buffers

    /* PI values to test */
    float kp_buf[20] = {0};
    float ki_buf[20] = {0};
    float i_max_buf[20] = {0};

    /* "Static" variables */
    int pi_state = 0;                                      // do not change
    char sd_filepath[100] = {0};                           // do not change
    unsigned int current_cycle = 0;                        // do not change
    unsigned long timestamp_testing_pi = 0;                // do not change
    unsigned long timestamp_last_update = millis() + 1000; // do not change
    unsigned long timestamp_print_status = 60 * 1000;      // do not change
    uint8_t done = 0;                                      // readout only. do not change
} PID_ControllerSweepData;

uint8_t pi_sweep_update(PI_CONTROLLER *pi, PID_ControllerSweepData *data);

/*Thermistors*/
#define AMOUNT_NTC_THERMISTORS 8 // Number of NTC probes
extern volatile char temp_init;
void temp_setup();
float temp_read_one(const uint8_t NTC, const uint8_t nTimes = 100, const float spike_tolerance = 1);
void temp_read_all(float buffer[AMOUNT_NTC_THERMISTORS]);
uint8_t temp_isconnected(uint8_t NTC = 255);

/*Oxygen Sensors*/
extern SerialPIO oxySerial;
extern volatile char oxy_serial_init;
void oxy_serial_setup();
void oxy_console();
uint8_t oxy_read_all(struct OxygenReadout measure_buffer[6]);
char *oxy_commandhandler(const char command[], uint8_t returnValues = 0);
uint8_t oxy_isconnected(const int PROBE = 255);

/*light spectrometers*/
extern volatile char light_init;
void light_setup();
void light_read(float *buffer, bool with_flash = 0);

/*error handeling*/
enum ERROR_DESTINATION
{
    ERROR_DESTINATION_NO_TCP,
    ERROR_DESTINATION_NO_SD,
    ERROR_DESTINATION_NO_TCP_SD
};
void error_handler(const int ErrorCode, int destination = 0);

#endif
