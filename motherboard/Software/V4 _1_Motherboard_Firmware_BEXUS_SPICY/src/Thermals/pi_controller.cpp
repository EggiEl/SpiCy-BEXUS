#include "header.h"
#include "debug_in_color.h"

// expanation on the used anti-windeup https://www.youtube.com/watch?v=NVLXCwc8HzM

PI_CONTROLLER pi_probe0 = {
    .desired_temp = SET_TEMP_DEFAULT,
    .heater_pin = PIN_H0,
    .thermistor_pin = NTC_OR_OxY_0,
    .kp = KP_DEFAULT,
    .ki = KI_DEFAULT,
    .I_MAX = I_MAX_DEFAULT,
    .PI_MAX_PW = PI_MAX_DEFAULT};

PI_CONTROLLER pi_probe1 = {
    .desired_temp = SET_TEMP_DEFAULT,
    .heater_pin = PIN_H1,
    .thermistor_pin = NTC_OR_OxY_1,
    .kp = KP_DEFAULT,
    .ki = KI_DEFAULT,
    .I_MAX = I_MAX_DEFAULT,
    .PI_MAX_PW = PI_MAX_DEFAULT};

PI_CONTROLLER pi_probe2 = {
    .desired_temp = SET_TEMP_DEFAULT,
    .heater_pin = PIN_H2,
    .thermistor_pin = NTC_OR_OxY_2,
    .kp = KP_DEFAULT,
    .ki = KI_DEFAULT,
    .I_MAX = I_MAX_DEFAULT,
    .PI_MAX_PW = PI_MAX_DEFAULT};

PI_CONTROLLER pi_probe3 = {
    .desired_temp = SET_TEMP_DEFAULT,
    .heater_pin = PIN_H3,
    .thermistor_pin = NTC_OR_OxY_3,
    .kp = KP_DEFAULT,
    .ki = KI_DEFAULT,
    .I_MAX = I_MAX_DEFAULT,
    .PI_MAX_PW = PI_MAX_DEFAULT};

PI_CONTROLLER pi_probe4 = {
    .desired_temp = SET_TEMP_DEFAULT,
    .heater_pin = PIN_H4,
    .thermistor_pin = NTC_4,
    .kp = KP_DEFAULT,
    .ki = KI_DEFAULT,
    .I_MAX = I_MAX_DEFAULT,
    .PI_MAX_PW = PI_MAX_DEFAULT};

PI_CONTROLLER pi_probe5 = {
    .desired_temp = SET_TEMP_DEFAULT,
    .heater_pin = PIN_H5,
    .thermistor_pin = NTC_5,
    .kp = KP_DEFAULT,
    .ki = KI_DEFAULT,
    .I_MAX = I_MAX_DEFAULT,
    .PI_MAX_PW = PI_MAX_DEFAULT};

void pi_update_controller(PI_CONTROLLER *pi);

/*initialises and updates all prprogrammed pi controller*/
void pi_update_all()
{
    /*update controller*/
    static long TimeStampPid = millis() + PI_T;
    if (millis() > TimeStampPid)
    {
        TimeStampPid = millis() + PI_T;
        // debugf_status("pi udate all\n");
        pi_update_controller(&pi_probe0);
        pi_update_controller(&pi_probe1);
        pi_update_controller(&pi_probe2);
        pi_update_controller(&pi_probe3);
        pi_update_controller(&pi_probe4);
        pi_update_controller(&pi_probe5);
    }
}

/*prints relevant data from PI_CONTROLLER * pi points to*/
void pi_print_controller(PI_CONTROLLER *pi)
{
    if (pi->kp == -100000.0)
    {
        debugf_warn("!disabled!:");
    }
    debugf_status("Pi|HE%u|NT%u|", pi->heater_pin, pi->thermistor_pin);
    debugf_info("Kp%.2f|Ki%f|I_MAX%f|TSET%.2f|PI_PW%.3f|", pi->kp, pi->ki, pi->I_MAX, pi->desired_temp, pi->PI_MAX_PW);
    debugf_info("i%f|pi%.3f|err%.2f|\n", pi->i_last, pi->pi_last, pi->error_last);
}

/**
 * updates the discrete PI controller and therefore the heating power as well.
 */
void pi_update_controller(PI_CONTROLLER *pi)
{
    if (!heat_init)
    {
        heat_setup();
    }

    if (pi->kp == -100000.0)
    {
        return;
    }

    /*static Variables*/
    /*Variables.*/
    unsigned long time_curr = millis();
    float measured_temp = temp_read_one(pi->thermistor_pin);
    /*dosn´t update controller if heater is not connectoed*/
    if (measured_temp == -1000000.00)
    {
        debugf_error("NTC %u not connected",pi->thermistor_pin);
        heat_updateone(pi->heater_pin, 0.0);
        return;
    }

    float error = pi->desired_temp - measured_temp;
    float dt = (time_curr - pi->time_last) / 1000.0f; // divided by 1000 because then its in s and not ms
    /*p*/
    float p = pi->kp * error;
    /*i*/
    float i = 0.5f * pi->ki * dt * (error + pi->error_last);

    /* Anti-wind-up */
    if (pi->pi_last == pi->PI_MAX_PW)
    // clamps integral when Controller is in positive saturation
    //  and i is still increasing (making things worse)
    {
        if (i > 0)
        {
            i = pi->i_last;
        }
    }
    else if (pi->pi_last == (-1 * pi->PI_MAX_PW))
    // clamps integral when Controller is in negative saturation
    //  and i is still decreasing (making things worse)
    {
        if (i < 0)
        {
            i = pi->i_last;
        }
    }
    else
    // increases i by the last value if non of the anti wind up occurs.
    {
        i = i + pi->i_last;
    }

    /*integral clamping*/
    if (i > pi->I_MAX)
    // This clamping isn´t depended on anything. Shouldn´t be needed in normal operation,
    //  but prevents exessive integral values if something gets wrong
    {
        i = pi->I_MAX;
    }
    else if (i < (-1 * pi->I_MAX))
    {
        i = (-1 * pi->I_MAX);
    }

    /*adding all up*/
    float pid = p + i;

    /*clamping PID output*/
    if (pid > pi->PI_MAX_PW)
    /*This is needed as the heating power provided can´t be infinite large*/
    {
        pid = pi->PI_MAX_PW;
    }
    else if (pid <= -pi->PI_MAX_PW)
    {
        pid = -pi->PI_MAX_PW;
    }

    /*converting PID to dutycycle for heater*/
    float heat = (100 * pid / HEAT_POWER);

    /*clamping heat*/
    if (heat <= 0)
    {
        heat = 0;
    }

    heat_updateone(pi->heater_pin, heat);

    pi->time_last = millis();
    pi->i_last = i;
    pi->error_last = error;
    pi->pi_last = pid;
    pi->heat = heat;
    // debugf_status("updating PIN_HEAT:%u with PIN_NTC:%u ", heater, thermistor);
    // debugf_info("SET_TEMP_DEFAULT:%.2f°C measure_temp:%.2f°C p:%.2f i:%.2f heat:%.2f%%\n", desired_temp, measured_temp, p, i, heat);
}

/**
 * records a transfer function with specified values.
 * First cooles Probes down to T_START.
 * Then waits and records till TIME_TILL_STOP
 * @return 0 is still recording 1 if done
 */
uint8_t pi_record_step_function(uint8_t PIN_HEATER, uint8_t PIN_NTC, float T_START, float TIME_TILL_STOP)
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
    static unsigned long timestamp_transfer_function = millis() + TIME_TILL_STOP;
    static unsigned long timestamp_last_update = millis() + 1000;
    static uint8_t done = 0;

    if (millis() > timestamp_last_update)
    /*limits frequency*/
    {
        timestamp_last_update = millis() + PI_T;

        /*measure temperature*/
        float temp_measure = temp_read_one(PIN_NTC);

        /*writes current temp to SD card*/
        char str_buff[300];
        if (pi_state != INIT)
        {
            snprintf(str_buff, 300, "%u,%f,%f", millis(), temp_measure, get_current());
            sd_writetofile(str_buff, sd_filepath);
        }

        /*prints status every status_delay s*/
        const unsigned long status_delay = 60 * 1000;
        static unsigned long timestamp_status = millis() + status_delay;
        if (millis() > timestamp_status)
        {
            timestamp_status = millis() + status_delay;
            debugf_status("pi_step_response[%u][%u][%f]°C Testing PID for: [%u]min\n", nMOTHERBOARD_BOOTUPS, PIN_HEATER, temp_measure, ((millis() - timestamp_transfer_function - (unsigned long)TIME_TILL_STOP)) / 1000 / 60);
        }

        switch (pi_state)
        {
        case INIT:
        {
            debugf_status("pi_step_response[%u][%u] state: Initialising\n", nMOTHERBOARD_BOOTUPS, PIN_HEATER);

            /*creating file path from bootups and heater*/
            snprintf(sd_filepath, 99, "temp_step_respons[%lu][%.2f].csv", nMOTHERBOARD_BOOTUPS, HEAT_POWER);

            /*creating header*/
            snprintf(str_buff, 300, "Timestamp,Temperatur,Power");
            sd_writetofile(str_buff, sd_filepath);

            debugf_status("pi_step_response[%u][%u] state: COOLING\n", nMOTHERBOARD_BOOTUPS, PIN_HEATER);
            pi_state = COOLING;
            break;
        }
        case COOLING:
            /*cools probe down till TEMP_COOL*/
            {
                heat_updateone(PIN_HEATER, 0.0);
                if (temp_measure < T_START)
                {
                    debugf_status("pi_step_response[%u][%u] state: RECORD_STEP\n", nMOTHERBOARD_BOOTUPS, PIN_HEATER);
                    pi_state = RECORD_STEP;
                    timestamp_transfer_function = millis() + TIME_TILL_STOP;
                }
                break;
            }
        case RECORD_STEP:
            /*switches on a PI controller with preprogrammed values for TIME_TILL_STOP*/
            {
                if (millis() < timestamp_transfer_function)
                // test if finished with all cycles
                {
                    heat_updateone(PIN_HEATER, 100.0);
                }
                else
                {
                    if (!done)
                    /*limits "done" print state*/
                    {
                        debugf_status("pi_step_response[%u][%u] state: Done\n", nMOTHERBOARD_BOOTUPS, PIN_HEATER);
                        heat_updateone(PIN_HEATER, 0.0);
                        done = 1;
                    }
                }
                break;
            }

        default:
            break;
        }
    }
    return done;
}

/**
 * records a predefined amoout of PI controller
 * @param pi specify the used pi controller here
 * @param data  specify the parameters of your sweep here
 */
uint8_t pi_sweep_update(PI_CONTROLLER *pi, PID_ControllerSweepData *data)
{
    enum pi_sweep_states
    {
        INIT,      // writes header and infos on the sd card
        COOLING,   // Waits till probe reaches TEMP_COOL
        TESTING_PI // testing the PI controller for TIME_TILL_STOP more milliseconds
    };

    if (millis() > data->timestamp_last_update)
    /*limits frequency*/
    {
        data->timestamp_last_update = millis() + PI_T;

        if (data->current_cycle < data->nCYCLES)
        // test if finished with all cycles
        {
            /*reads out temperature*/
            float temp_measure = temp_read_one(pi->thermistor_pin);

            /*writes current temp and power to SD card*/
            char str_buff[300];
            if (data->pi_state != INIT)
            {
                snprintf(str_buff, 300, "%u,%f,%f,%f", millis(), temp_measure, pi->pi_last, pi->i_last);
                sd_writetofile(str_buff, data->sd_filepath);
            }

            /*prints status every status_delay ms*/
            if (millis() > data->timestamp_print_status)
            {
                data->timestamp_print_status = millis() + PI_SWEEP_PRINT_DELAY;
                if (data->pi_state == COOLING)
                {
                    debugf_info("pi[%u][%u][%u][%f°C] Cooling...\n", nMOTHERBOARD_BOOTUPS, pi->heater_pin, data->current_cycle, temp_measure);
                }
                else if (data->pi_state == TESTING_PI)
                {
                    debugf_info("pi[%u][%u][%u][%.2fmin][%f°C] Testing PID for: [%f]min\n", nMOTHERBOARD_BOOTUPS, pi->heater_pin, data->current_cycle, (float)(millis() - (data->timestamp_testing_pi - data->TIME_TILL_STOP)) / 1000.0 / 60.0, temp_measure, (float)(data->TIME_TILL_STOP / 1000 / 60));
                }
            }

            switch (data->pi_state)
            /*finite state mashine*/
            {
            case INIT:
            {
                debugf_status("pi[%u][%u] state: Initialising\n", nMOTHERBOARD_BOOTUPS, pi->heater_pin);

                /*creating file path from bootups and heater*/
                snprintf(data->sd_filepath, 99, "temp_log[%lu][%u].csv", nMOTHERBOARD_BOOTUPS, pi->heater_pin);

                /*saves Kp Ki Ki_max values*/
                char str_buff_big[500];
                snprintf(str_buff_big, 300, "!PID_CONTROLLER_SWEEP!\nnCycle %u,TEMP_COOL %.2f, TEMP_SET %.2f, time_till_stop %lu\n", data->nCYCLES, data->TEMP_COOL, data->TEMP_SET, data->TIME_TILL_STOP);
                for (uint8_t a = 0; a < data->nCYCLES; a++)
                // copies each value in a string
                {
                    snprintf(str_buff, 300, "|%u|Kp:%f Ki:%f Ki_max:%f\n", a, data->kp_buf[a], data->ki_buf[a], data->i_max_buf[a]);
                    strncat(str_buff_big, str_buff, 500 - strlen(str_buff_big) - 1);
                }
                sd_writetofile(str_buff_big, data->sd_filepath);

                /*creating header*/
                snprintf(str_buff, 300, "Timestamp,Temperatur,Power,Integral");
                sd_writetofile(str_buff, data->sd_filepath);

                /*printing infp*/
                debugf_status("%s\n", str_buff);

                debugf_status("pi[%u][%u][%u] state: Cooling\n", nMOTHERBOARD_BOOTUPS, pi->heater_pin, data->current_cycle);
                data->pi_state = COOLING;
                break;
            }
            case COOLING:
                /*cools probe down till TEMP_COOL*/
                {
                    heat_updateone(pi->heater_pin, 0.0);
                    if (temp_measure < data->TEMP_COOL)
                    {
                        debugf_status("pi[%u][%u][%u] state: TESTING_PI\n", nMOTHERBOARD_BOOTUPS, pi->heater_pin, data->current_cycle);
                        data->pi_state = TESTING_PI;
                        pi->kp = data->kp_buf[data->current_cycle];
                        pi->ki = data->ki_buf[data->current_cycle];
                        pi->I_MAX = data->i_max_buf[data->current_cycle];
                        pi->desired_temp = data->TEMP_SET;
                        data->timestamp_testing_pi = millis() + data->TIME_TILL_STOP;
                        pi_print_controller(pi);
                    }
                    break;
                }
            case TESTING_PI:
                /*switches on a PI controller with preprogrammed values for TIME_TILL_STOP*/
                {
                    pi_update_controller(pi);
                    if (millis() > data->timestamp_testing_pi)
                    {
                        debugf_status("pi[%u][%u][%u] state: Cooling\n", nMOTHERBOARD_BOOTUPS, pi->heater_pin, data->current_cycle);
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
                debugf_status("pi[%u][%u] state: Done\n", nMOTHERBOARD_BOOTUPS, pi->heater_pin);
                heat_updateone(pi->heater_pin, 0.0);
                data->done = 1;
            }
        }
    }
    return data->done;
}
