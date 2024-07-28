#include "header.h"
#include "debug_in_color.h"
const static float MAX_HEAT_POWER = HEAT_CURRENT * HEAT_CURRENT * HEAT_RESISTANCE;
static float PID_MAX = 2.5;
static float I_BUFFER_MAX = PID_MAX * 0.2;
const unsigned long PID_T = 1000; // time in ms till next PID controller update
float SET_TEMP = 33.0;

float kp = 10;
float ki = 0;
float kd = 0;

void pid_update_one(float desired_temp, uint8_t heater, uint8_t thermistor);

volatile char pid_init = 0;
void pid_setup()
{
    if (!heat_init)
    {
        heat_setup();
    }

    pid_init = 1;
}

void pid_update_all()
{
    if (!pid_init)
    {
        pid_setup();
    }
    static long TimeStampPid = millis() + PID_T;
    if (millis() > TimeStampPid)
    {
        TimeStampPid = millis() + PID_T;
        // debugf_blue("pid pudate\n");
        pid_update_one(SET_TEMP, PIN_H0, NTC_PROBE_0);
        // pid_update_one(SET_TEMP, PIN_H1, NTC_PROBE_1);
        // pid_update_one(SET_TEMP, PIN_H2, NTC_PROBE_2);
        // pid_update_one(SET_TEMP, PIN_H3, NTC_PROBE_3);
        // pid_update_one(SET_TEMP, PIN_H4, NTC_4);
        // pid_update_one(SET_TEMP, PIN_H5, NTC_5);
    }
}

void pid_update_one(float desired_temp, uint8_t heater, uint8_t thermistor)
{
    if (!pid_init)
    {
        pid_setup();
    }
    /*static Variables*/
    static float I_buffer = 0;
    static unsigned long last_measurement = millis();

    /*Variables.*/
    unsigned long current_measurement = millis();
    float measured_temp = temp_read_one(thermistor);
    float error = desired_temp - measured_temp;
    /*p*/
    float p = kp * error;
    /*i*/
    I_buffer += ki* error * (current_measurement - last_measurement);
    if (I_buffer > I_BUFFER_MAX)
    {
        I_buffer = I_BUFFER_MAX;
    }
    float i = I_buffer;
    /*d*/
    float d = 0;
    /*adding all up*/
    float pid = p + i + d;
    if (pid < 0)
    {
        debugf_error("pid got negative values. Set to 0.\n");
        pid = 0;
    }

    if (pid > PID_MAX)
    {
        pid = PID_MAX;
    }
    if (pid < -1 * PID_MAX)
    {
        pid = -1 * PID_MAX;
    }

    /*converting PID to dutycycle for heater*/
    float heat = (100* pid / PID_MAX);

    heat_updateone(heater, heat);

    last_measurement = millis();
    debugf_info("PIN_HEAT:%u PIN_NTC:%u SET_TEMP:%.2f°C measure_temp:%.2f°C p:%.2f i:%.2f heat:%.2f%%\n", heater, thermistor, desired_temp, measured_temp, p, i, heat);
}