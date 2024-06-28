#include "header.h"
static float PID_MAX;
float PID_FREQ = 100.0;

char pid_init = 0;
void pid_setup()
{
    pid_init = 1;
}

float kp = 0;
float ki = 0;
float kd = 0;

void pid_update_one(float desired_temp, uint8_t heater, uint8_t thermistor)
{
    /*static Variables*/
    static long TimeStampPid = millis();
    static float I_buffer = 0;
    if (!pid_init)
    {
        pid_setup();
    }

    if (TimeStampPid < (1.0 / PID_FREQ))
    {
        /*Variables.*/
        float measured_temp = temp_read_one(thermistor);
        float error = desired_temp - measured_temp;
        /*p*/
        float p = kp * error;
        /*i*/
        I_buffer += error;
        float i = I_buffer;
        /*d*/
        float d = 0;
        /*calculations*/
        float pid = p + i + d;

        uint16_t heat = (int)(ADC_MAX_WRITE * pid / PID_MAX);
        debugf_info("heater:%u thermistor:%u setpoint:%.2f currentTemp:%.2f p:%.2f i:%.2f heat:%u", heater, thermistor, desired_temp, measured_temp, p, i, heat);
        heat_updateone(heater, heat);
    }
}

void ringbuffer()
{
}