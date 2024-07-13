#include "header.h"
#include "debug_in_color.h"
static float PID_MAX = ADC_MAX_WRITE;
static float I_BUFFER_MAX = PID_MAX * 0.2;
float PID_FREQ = 100.0;
float SET_TEMP = 30.0;

float kp = 0;
float ki = 0;
float kd = 0;

void pid_update_one(float desired_temp, uint8_t heater, uint8_t thermistor);

volatile char pid_init = 0;
void pid_setup()
{
    heat_setup();
    pid_init = 1;
}

void pid_update_all()
{
    if (!pid_init)
    {
        pid_setup();
    }
    static long TimeStampPid = millis();
    if (TimeStampPid < (1.0 / PID_FREQ))
    {
        pid_update_one(SET_TEMP, PIN_H0, NTC_PROBE_0);
        pid_update_one(SET_TEMP, PIN_H1, NTC_PROBE_1);
        pid_update_one(SET_TEMP, PIN_H2, NTC_PROBE_2);
        pid_update_one(SET_TEMP, PIN_H3, NTC_PROBE_3);
        pid_update_one(SET_TEMP, PIN_H4, NTC_PROBE_4);
        pid_update_one(SET_TEMP, PIN_H5, NTC_PROBE_5);
    }
}

void pid_update_one(float desired_temp, uint8_t heater, uint8_t thermistor)
{
    /*static Variables*/
    static float I_buffer = 0;

    /*Variables.*/
    float measured_temp = temp_read_one(thermistor);
    float error = desired_temp - measured_temp;
    /*p*/
    float p = kp * error;
    /*i*/
    I_buffer += error;
    if (I_buffer > I_BUFFER_MAX)
    {
        I_buffer = I_BUFFER_MAX;
    }
    float i = I_buffer;
    /*d*/
    float d = 0;
    /*calculations*/
    float pid = p + i + d;
    if (pid < 0)
    {
        debugf_error("pid got negative values. Set to 0.\n");
        pid = 0;
    }
    float heat = (ADC_MAX_WRITE * pid / PID_MAX);
    debugf_info("heater:%u thermistor:%u setpoint:%.2f currentTemp:%.2f p:%.2f i:%.2f heat:%u", heater, thermistor, desired_temp, measured_temp, p, i, heat);
    heat_updateone(heater, heat);
}