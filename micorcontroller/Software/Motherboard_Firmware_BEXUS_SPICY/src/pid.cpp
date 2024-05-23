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

void pid_update_one(float SetTemp, uint8_t Heater, uint8_t Thermistor)
{
    /*static Variables*/
    static long TimeStampPid = millis();
    static float I_buffer = 0;
    if (!pid_init)
    {
        pid_setup();
    }

    if (TimeStampPid < (1 / PID_FREQ))
    {
        /*Variables.*/
        float CurrentTemp = temp_read_one(Thermistor);
        float error = SetTemp - CurrentTemp;
        /*p*/
        float p = kp * error;
        /*i*/
        I_buffer += error;
        float i = I_buffer;
        /*d*/
        float d = 0;
        /*calculations*/
        float pid = p + i + d;

        float heat = HEAT_HUNDERTPERCENT * pid / PID_MAX;
        heat_updateone(Heater, heat);
    }
}

void ringbuffer()
{
}