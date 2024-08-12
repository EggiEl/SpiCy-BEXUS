#include "header.h"
#include "debug_in_color.h"

// expanation on the used anti-windeup https://www.youtube.com/watch?v=NVLXCwc8HzM
const static float MAX_HEAT_POWER = HEAT_CURRENT * HEAT_CURRENT * HEAT_RESISTANCE;
const float PID_MAX = 2.5;
float I_MAX = 3;
const unsigned long PID_T = 200; // time in ms till next PID controller update
float SET_TEMP = 34.0;

float kp = 10;
float ki = 0.1;

void pid_update_one(float desired_temp, uint8_t heater, uint8_t thermistor);

void pid_update_all()
{
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
    if (!heat_init)
    {
        heat_setup();
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
    float dt = (time_curr - time_last) / 1000.0f; // divided by 1000 because then its in s and not ms
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
    // This clamping isn´t depended on anything. Shouldn´t be needed in normal operation,
    //  but prevents exessive integral values if something gets wrong
    {
        i = I_MAX;
    }
    else if (i < (-1 * I_MAX))
    {

        i = (-1 * I_MAX);
    }

    /*adding all up*/
    float pid = p + i;

    /*clamping PID output*/
    if (pid > PID_MAX)
    /*This is needed as the heating power provided can´t be infinite large*/
    {
        pid = PID_MAX;
    }
    else if (pid <= -PID_MAX)
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
void pid_controller_sweep(PID_ControllerSweepData *data)
{

    if (millis() > data->timestamp_last_update)
    /*limits frequency at 1 Hz*/
    {
        data->timestamp_last_update = millis() + 1000;

        if (data->current_cycle < data->nCYCLES)
        // test if finished with all cycles
        {
            /*prints status every status_delay s*/
            const unsigned long status_delay = 60 * 1000;
            unsigned long timestamp_status = millis() + status_delay;
            if (millis() > timestamp_status)
            {
                timestamp_status = millis() + status_delay;
                if (data->pi_state == COOLING)
                {
                    debugf_status("pi[%u][%u][%u] Cooling...\n", nMOTHERBOARD_BOOTUPS, data->Heater, data->current_cycle);
                }
                else if (data->pi_state == TESTING_PI)
                {
                    debugf_status("pi[%u][%u][%u] Testing PID for: [%u]min\n", nMOTHERBOARD_BOOTUPS, data->Heater, data->current_cycle, (millis() - data->timestamp_testing_pi - data->TIME_TILL_STOP) / 1000.0 / 60.0);
                }
            }

            /*writes current temp to SD card*/
            char str_buff[300];
            float temp_measure = temp_read_one(data->NTC);
            if (data->pi_state != INIT)
            {
                snprintf(str_buff, 300, "%u,%f,", millis(), temp_measure);
                sd_writetofile(str_buff, data->sd_filepath);
            }

            switch (data->pi_state)
            {
            case INIT:
            {
                debugf_status("pi[%u][%u] state: Initialising\n", nMOTHERBOARD_BOOTUPS, data->Heater);
                /*checks if all buffers are the right size*/
                // if (!(sizeof(data->kp_buffer) / sizeof(float) == data->nCYCLES && sizeof(data->ki_buffer) / sizeof(float) == data->nCYCLES && data->nCYCLES == data->nCYCLES))
                // {
                //     debugf_error("Some Controller gain buffer is wrong\n");
                //     return;
                // }

                /*creating file path from bootups and heater*/
                snprintf(data->sd_filepath, 99, "temp_log[%lu][%u].csv", nMOTHERBOARD_BOOTUPS, data->Heater);

                /*saves Kp Ki Ki_max values*/
                char str_buff_big[500];
                snprintf(str_buff_big, 300, "!PID_CONTROLLER_SWEEP!\nnCycle %u,TEMP_COOL %.2f, TEMP_SET %.2f, time_till_stop %lu\n", data->nCYCLES, data->TEMP_COOL, data->TEMP_SET, data->TIME_TILL_STOP);
                for (uint8_t a = 0; a < data->nCYCLES; a++)
                // copies each value in a string
                {
                    snprintf(str_buff, 300, "|%u|Kp:%f Ki:%f Ki_max:%f\n", a, data->kp_buffer[a], data->ki_buffer[a], data->ki_max_buffer[a]);
                    strncat(str_buff_big, str_buff, 500 - strlen(str_buff_big) - 1);
                }
                sd_writetofile(str_buff_big, data->sd_filepath);

                /*creating header*/
                snprintf(str_buff, 300, "Timestamp,Temperatur");
                sd_writetofile(str_buff, data->sd_filepath);

                debugf_status("pi[%u][%u][%u] state: Cooling\n", nMOTHERBOARD_BOOTUPS, data->Heater, data->current_cycle);
                data->pi_state = COOLING;
                break;
            }
            case COOLING:
                /*cools probe down till TEMP_COOL*/
                {
                    heat_updateone(data->Heater, 0.0);
                    if (temp_measure < data->TEMP_COOL)
                    {
                        debugf_status("pi[%u][%u][%u] state: TESTING_PI\n", nMOTHERBOARD_BOOTUPS, data->Heater, data->current_cycle);
                        data->pi_state = TESTING_PI;
                        data->timestamp_testing_pi = millis() + data->TIME_TILL_STOP;
                    }
                    break;
                }
            case TESTING_PI:
                /*switches on a PI controller with preprogrammed values for TIME_TILL_STOP*/
                {
                    kp = data->kp_buffer[data->current_cycle];
                    ki = data->ki_buffer[data->current_cycle];
                    I_MAX = data->ki_max_buffer[data->current_cycle];
                    pid_update_one(data->TEMP_SET, data->Heater, data->NTC);

                    if (millis() > data->timestamp_testing_pi)
                    {
                        debugf_status("pi[%u][%u][%u] state: Cooling\n", nMOTHERBOARD_BOOTUPS, data->Heater, data->current_cycle);
                        data->pi_state = COOLING;
                        data->current_cycle++;
                    }
                    break;
                }
            default:
                break;
            }
        }
        else
        {
            if (!data->done)
            /*limits "done" print state*/
            {
                debugf_status("pi[%u][%u] state: Done\n", nMOTHERBOARD_BOOTUPS, data->Heater);
                heat_updateone(data->Heater, 0.0);
                data->done;
            }
        }
    }
}

// /**
//  * - cools down to TEMP_COOL
//  * - engages a pi controller with preprogrmmed values
//  * - when temp hits TEMP_SET -> waits for TIME_TILL_STOP
//  * - cooling down again
//  * repeats this cycle a fixed amout with different ki values
//  */
void pid_controller_sweep_simple(uint8_t Heater, uint8_t NTC)
{
    enum pi_sweep_states
    {
        INIT,      // writes header and infos on the sd card
        COOLING,   // Waits till probe reaches TEMP_COOL
        TESTING_PI // testing the PI controller for TIME_TILL_STOP more milliseconds
    };

    /*controll parameters*/
    const float TEMP_COOL = 31;
    const float TEMP_SET = 32;
    const unsigned long TIME_TILL_STOP = 0.5 * (60 * 60 * 1000); // in h
    const unsigned int nCYCLES = 10;

    /*Pi values to test*/
    const float kp_buffer[] = {4, 10, 4, 10, 4, 10, 4, 10, 4, 10};
    const float ki_buffer[] = {0.01, 0.01, 0.1, 0.1, 1, 1, 10, 10, 100, 100}; // 0, 0.00001, 0.0001, 0.001, 0.01, 0.1, 1, 10, 100, 1000
    const float ki_max_buffer[] = {0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2};

    /*static variables*/
    static enum pi_sweep_states pi_state = INIT;
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
            /*prints status every status_delay s*/
            const unsigned long status_delay = 60 * 1000;
            unsigned long timestamp_status = millis() + status_delay;
            if (millis() > timestamp_status)
            {
                timestamp_status = millis() + status_delay;
                if (pi_state == COOLING)
                {
                    debugf_status("pi[%u][%u][%u] Cooling...\n", nMOTHERBOARD_BOOTUPS, Heater, current_cycle);
                }
                else if (pi_state == TESTING_PI)
                {
                    debugf_status("pi[%u][%u][%u] Testing PID for: [%u]min\n", nMOTHERBOARD_BOOTUPS, Heater, current_cycle, (millis() - timestamp_testing_pi - TIME_TILL_STOP) / 1000.0 / 60.0);
                }
            }

            /*writes current temp to SD card*/
            char str_buff[300];
            float temp_measure = temp_read_one(NTC);
            if (pi_state != INIT)
            {
                snprintf(str_buff, 300, "%u,%f,", millis(), temp_measure);
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
            static uint8_t done = 0;
            if (!done)
            /*limits "done" print state*/
            {
                debugf_status("pi[%u][%u] state: Done\n", nMOTHERBOARD_BOOTUPS, Heater);
                heat_updateone(Heater, 0.0);
                done++;
            }
        }
    }
}

uint8_t pid_record_tranfer_function(uint8_t Heater, uint8_t NTC, float T_START, float TIME_TILL_STOP)
{
    enum pi_sweep_states
    {
        INIT, // writes header and infos on the sd card
        COOLING,
        RECORD_STEP
    };

    /*static variables*/
    static enum pi_sweep_states pi_state = INIT;
    static char sd_filepath[100];
    static unsigned long timestamp_tranfer_function = millis() + TIME_TILL_STOP;
    static unsigned long timestamp_last_update = millis() + 1000;
    static uint8_t done = 0;

    if (millis() > timestamp_last_update)
    /*limits frequency at 1 Hz*/
    {
        timestamp_last_update = millis() + 1000;

        if (millis() < timestamp_tranfer_function)
        // test if finished with all cycles
        {
            float temp_measure = temp_read_one(NTC);

            /*prints status every status_delay s*/
            const unsigned long status_delay = 60 * 1000;
            unsigned long timestamp_status = millis() + status_delay;
            if (millis() > timestamp_status)
            {
                timestamp_status = millis() + status_delay;
                debugf_status("pi_step_response[%u][%u][%f] Testing PID for: [%u]min\n", nMOTHERBOARD_BOOTUPS, Heater, temp_measure, (millis() - timestamp_tranfer_function - TIME_TILL_STOP) / 1000.0 / 60.0);
            }

            /*writes current temp to SD card*/
            char str_buff[300];

            if (pi_state != INIT)
            {
                snprintf(str_buff, 300, "%u,%f,%f", millis(), temp_measure, get_current());
                sd_writetofile(str_buff, sd_filepath);
            }

            switch (pi_state)
            {
            case INIT:
            {
                debugf_status("pi_step_response[%u][%u] state: Initialising\n", nMOTHERBOARD_BOOTUPS, Heater);

                /*creating file path from bootups and heater*/
                snprintf(sd_filepath, 99, "temp_step_respons[%lu][%.2f].csv", nMOTHERBOARD_BOOTUPS, HEAT_VOLTAGE * HEAT_CURRENT);

                /*creating header*/
                snprintf(str_buff, 300, "Timestamp,Temperatur,Power");
                sd_writetofile(str_buff, sd_filepath);

                debugf_status("pi_step_response[%u][%u] state: COOLING\n", nMOTHERBOARD_BOOTUPS, Heater);
                pi_state = COOLING;
                break;
            }
            case COOLING:
                /*cools probe down till TEMP_COOL*/
                {
                    heat_updateone(Heater, 0.0);
                    if (temp_measure < T_START)
                    {
                        debugf_status("pi_step_response[%u][%u] state: RECORD_STEP\n", nMOTHERBOARD_BOOTUPS, Heater);
                        pi_state = RECORD_STEP;
                        timestamp_tranfer_function = millis() + TIME_TILL_STOP;
                    }
                    break;
                }
            case RECORD_STEP:
                /*switches on a PI controller with preprogrammed values for TIME_TILL_STOP*/
                {
                    heat_updateone(Heater, 100.0);
                    break;
                }
            default:
                break;
            }
        }
        else
        {
            if (!done)
            /*limits "done" print state*/
            {
                debugf_status("pi_step_response[%u][%u] state: Done\n", nMOTHERBOARD_BOOTUPS, Heater);
                heat_updateone(Heater, 0.0);
                done = 1;
            }
        }
    }
    return done;
}
