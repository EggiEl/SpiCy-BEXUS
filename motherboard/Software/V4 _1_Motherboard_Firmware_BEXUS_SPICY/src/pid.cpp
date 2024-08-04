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
    // debugf_status("updating PIN_HEAT:%u with PIN_NTC:%u ", heater, thermistor);
    // debugf_info("SET_TEMP:%.2f°C measure_temp:%.2f°C p:%.2f i:%.2f heat:%.2f%%\n", desired_temp, measured_temp, p, i, heat);
}

/**
 * - cools down to TEMP_COOL
 * - engages a pi controller with preprogrmmed values
 * - when temp hits TEMP_SET -> waits for TIME_TILL_STOP
 * - cooling down again
 * repeats this cycle a fixed amout with different ki values
 */
void pid_controller_sweep(uint8_t Heater, uint8_t NTC)
{
    enum pi_collect_states
    {
        INIT,      // writes header and infos on the sd card
        COOLING,   // Waits till probe reaches TEMP_COOL
        TESTING_PI // testing the PI controller for TIME_TILL_STOP more milliseconds
    };

    /*controll parameters*/
    const float TEMP_COOL = 31;
    const float TEMP_SET = 32;
    const unsigned long TIME_TILL_STOP = 1.5 * (60 * 60 * 1000); // in h
    const unsigned int nCYCLES = 10;

    /*Pi values to test*/
    const float kp_buffer[] = {0.1, 1, 5, 10, 4, 4, 4, 4, 4, 4};
    const float ki_buffer[] = {0, 0, 0, 0, 0, 0.0001, 0.0004, 0.001, 0.01, 0.1}; 
    const float ki_max_buffer[] = {0, 0, 0, 0, 0, 0.1, 0.1, 0.1, 0.1, 0.1};

    /*static variables*/
    static enum pi_collect_states pi_state = INIT;
    static char sd_filepath[100];
    static unsigned int current_cycle = 0;
    static unsigned long timestamp_testing_pi = 0;
    static unsigned long timestamp_last_update = millis() + 1000;

    if (millis() > timestamp_last_update)
    /*limits frequency at 1 Hz*/
    {
        timestamp_last_update = millis() + 1000;

        if (current_cycle < nCYCLES)
        // test if finished with all cycles
        {
            char str_buff[300];
            float temp_measure = temp_read_one(NTC);
            /*writes current temp to SD card*/
            if (pi_state != INIT)
            {
                snprintf(str_buff, 300, "%u,%.4f,", millis(), temp_measure);
                sd_writetofile(str_buff, sd_filepath);
            }

            switch (pi_state)
            {
            case INIT:
            {
                debugf_status("pi[%u][%u] state: Initialising\n", nMOTHERBOARD_BOOTUPS, Heater);
                /*checks if all buffers are the right size*/
                if (!(sizeof(kp_buffer) / sizeof(float) == nCYCLES && sizeof(ki_buffer) / sizeof(float) == nCYCLES && sizeof(ki_max_buffer) / sizeof(float) == nCYCLES))
                {
                    debugf_error("Some Controller gain buffer is wrong\n");
                    return;
                }

                /*creating file path from bootups and heater*/
                snprintf(sd_filepath, 99, "temp_log[%lu][%u].csv", nMOTHERBOARD_BOOTUPS, Heater);

                /*saves Kp Ki Ki_max values*/
                char str_buff_big[500];
                snprintf(str_buff_big, 300, "!PID_CONTROLLER_SWEEP!\nnCycle %u,TEMP_COOL %.2f, TEMP_SET %.2f, time_till_stop %lu\n", nCYCLES, TEMP_COOL, TEMP_SET, TIME_TILL_STOP);
                for (uint8_t a = 0; a < nCYCLES; a++)
                // copies each value in a string
                {
                    snprintf(str_buff, 300, "|%u|Kp:%f Ki:%f Ki_max:%f\n", a, kp_buffer[a], ki_buffer[a], ki_max_buffer[a]);
                    strncat(str_buff_big, str_buff, 500 - strlen(str_buff_big) - 1);
                }
                sd_writetofile(str_buff_big, sd_filepath);

                /*creating header*/
                snprintf(str_buff, 300, "Timestamp,Temperatur");
                sd_writetofile(str_buff, sd_filepath);

                debugf_status("pi[%u][%u][%u] state: Cooling\n", nMOTHERBOARD_BOOTUPS, Heater, current_cycle);
                pi_state = COOLING;
                break;
            }
            case COOLING:
                /*cools probe down till TEMP_COOL*/
                {
                    heat_updateone(Heater, 0.0);
                    if (temp_measure < TEMP_COOL)
                    {
                        debugf_status("pi[%u][%u][%u] state: TESTING_PI\n", nMOTHERBOARD_BOOTUPS, Heater, current_cycle);
                        pi_state = TESTING_PI;
                        timestamp_testing_pi = millis() + TIME_TILL_STOP;
                    }
                    break;
                }
            case TESTING_PI:
                /*switches on a PI controller with preprogrammed values for TIME_TILL_STOP*/
                {
                    kp = kp_buffer[current_cycle];
                    ki = ki_buffer[current_cycle];
                    I_MAX = ki_max_buffer[current_cycle];
                    pid_update_one(TEMP_SET, Heater, NTC);

                    // if ((millis() - (timestamp_testing_pi - TIME_TILL_STOP)) > (0.5 * TIME_TILL_STOP))
                    // /*print a info after halftime*/
                    // {
                    //     static uint8_t already_printed = 0;
                    //     if (!already_printed)
                    //     {
                    //         already_printed = 1;
                    //         debugf_status("pi[%u][%u][%u] state: TESTING_PI 50%% done\n", nMOTHERBOARD_BOOTUPS, Heater, current_cycle);
                    //     }
                    // }

                    if (millis() > timestamp_testing_pi)
                    {
                        debugf_status("pi[%u][%u][%u] state: Cooling\n", nMOTHERBOARD_BOOTUPS, Heater, current_cycle);
                        pi_state = COOLING;
                        current_cycle++;
                    }
                    break;
                }
            default:
                break;
            }
        }
        else
        {
            static uint8_t done_counter = 0;
            if (!done_counter)
            /*limits "done" print state*/
            {
                debugf_status("pi[%u][%u] state: Done\n", nMOTHERBOARD_BOOTUPS, Heater);
                heat_updateone(Heater, 0.0);
                done_counter++;
            }
        }
    }
}
