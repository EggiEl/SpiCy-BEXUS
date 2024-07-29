#include "header.h"
#include "debug_in_color.h"

// expanation on the used anti-windeup https://www.youtube.com/watch?v=NVLXCwc8HzM
const static float MAX_HEAT_POWER = HEAT_CURRENT * HEAT_CURRENT * HEAT_RESISTANCE;
const float PID_MAX = 2.5;
float I_MAX = 3;
const unsigned long PID_T = 200; // time in ms till next PID controller update
float SET_TEMP = 34.0;

float kp = 4;
float ki = 0.1;

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

/**
 * - cools down to temp_start
 * - engages a pi controller with preprogrmmed values
 * - when temp hits temp_stop -> waits for time_till_stop
 * - cooling down again
 * repeats this cycle a fixed amout with different ki values
 */
void pid_collect_samples(uint8_t Heater, uint8_t NTC)
{
    const float temp_start = 28;
    const float temp_stop = 32;
    const float time_till_stop = 10 * 60 * 1000; // ms
    const float cycles = 5;
    char sd_filepath[100];
    char string_buffer[300];
    
    //measurements 225 (should be?)
    kp = 4;
    float ki_buffer[] = {0.0004,0.0004,0.0004,0.0004,0.0004};
    float ki_max_buffer[] = {0.05,0.1,1,2,4};

    //measurements 223 (should be?)
    // float ki_buffer[] = {1, 0.1, 0.01, 0.001, 0.0001};
    // float ki_max_buffer[] = {1, 1, 1, 1, 1};

    snprintf(sd_filepath, 99, "temp_log[%u].csv", nMOTHERBOARD_BOOTUPS);

    /*creating header*/
    snprintf(string_buffer, 300, "Timestamp,Temperatur");
    sd_writetofile(string_buffer, sd_filepath);

    for (int a = 0; a < cycles; a++)
    {
        /*setting pi values*/
        ki = ki_buffer[a];
        I_MAX = ki_max_buffer[a];
        heat_updateone(Heater, 0.0);
        /*cooling till start temp*/
        while (temp_read_one(NTC) > temp_start)
        {
            checkSerialInput();
            snprintf(string_buffer, 300, "%u,%.2f,", millis(), temp_read_one(NTC));
            sd_writetofile(string_buffer, sd_filepath);
            delay(1000);
        }

        /*pi till stop temp*/
        while (temp_read_one(NTC) < temp_stop)
        {
            checkSerialInput();
            snprintf(string_buffer, 300, "%u,%.2f,", millis(), temp_read_one(NTC));
            sd_writetofile(string_buffer, sd_filepath);
            delay(1000);
            pid_update_one(temp_stop, Heater, NTC);
        }

        /*still recording for time_till_stop*/
        unsigned long timestamp = millis() + time_till_stop;
        while (millis() < time_till_stop)
        {
            checkSerialInput();
            snprintf(string_buffer, 300, "%u,%.2f,", millis(), temp_read_one(NTC));
            sd_writetofile(string_buffer, sd_filepath);
            pid_update_one(temp_stop, Heater, NTC);
            delay(1000);
        }
    }
    heat_updateone(Heater, 0.0);
}

void pid_update_one(float desired_temp, uint8_t heater, uint8_t thermistor)
{
    if (!pid_init)
    {
        pid_setup();
    }
    /*static Variables*/
    static float i_last = 0;
    static unsigned long time_last = millis();
    static float error_last = 0;
    static float pid_last = 0;

    /*Variables.*/
    unsigned long time_curr = millis();
    float measured_temp = temp_read_one(thermistor);
    float error = desired_temp - measured_temp;
    float dt = (time_curr - time_last) / 1000.0f;
    /*p*/
    float p = kp * error;
    /*i*/
    float i = 0.5f * ki * dt * (error + error_last);

    /* Anti-wind-up */
    if (pid_last == PID_MAX)
    // clamps integral when Controller is in positive saturation
    //  and i is still increasing (making things worse)
    {
        if (i > 0)
        {
            i = i_last;
        }
    }
    else if (pid_last == (-1 * PID_MAX))
    // clamps integral when Controller is in negative saturation
    //  and i is still decreasing (making things worse)
    {
        if (i < 0)
        {
            i = i_last;
        }
    }
    else
    // increases i by the last value if non of the anti wind up occurs.
    {
        i = i + i_last;
    }

    /*integral clamping*/
    if (i > I_MAX)
    {
        i = I_MAX;
    }
    else if (i < (-1 * I_MAX))
    {

        i = (-1 * I_MAX);
    }

    /*adding all up*/
    float pid = p + i;
    /*claping PID output*/
    if (pid > PID_MAX)
    {
        pid = PID_MAX;
    }
    if (pid <= -PID_MAX)
    {
        pid = -PID_MAX;
    }

    /*converting PID to dutycycle for heater*/
    float heat = (100 * pid / PID_MAX);

    /*claping heat*/
    if (heat <= 0)
    {
        heat = 0;
    }

    heat_updateone(heater, heat);

    time_last = millis();
    i_last = i;
    error_last = error;
    pid_last = pid;
    debugf_status("updating PIN_HEAT:%u with PIN_NTC:%u ", heater, thermistor);
    debugf_info("SET_TEMP:%.2f°C measure_temp:%.2f°C p:%.2f i:%.2f heat:%.2f%%\n", desired_temp, measured_temp, p, i, heat);
}