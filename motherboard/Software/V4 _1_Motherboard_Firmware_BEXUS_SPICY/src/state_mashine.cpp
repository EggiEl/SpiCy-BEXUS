#include "header.h"
#include "debug_in_color.h"

void state_print(unsigned int Status);

enum states
{
    START,
    CLEAR_PACKET,
    READ_TEMP,
    READ_OXY,
    READ_LIGHT,
    SAVESENDPACKET,
    ERROR
};

/*
 * Changes the current state to the next one
 */
void next_state()
{
    static enum states state = START;
    static struct packet packet_dl = {0}; /*packet wich will be the next downlink*/
    static struct OxygenReadout mesure_buffer[6] = {0};

    static char sd_filepath[100];
    state_print(state);
    switch (state)
    {
    case START:
    {
        state = CLEAR_PACKET;
        snprintf(sd_filepath, 99, "data[%lu].bin", nMOTHERBOARD_BOOTUPS);
        break;
    }
    case CLEAR_PACKET:
    {
        state = READ_TEMP;
        struct packet *new_packet = packet_create();
        memcpy(&packet_dl, new_packet, sizeof(struct packet));
        free(new_packet);
        break;
    }
    case READ_TEMP:
    {
        /*temp*/
        pause_Core1();
        float buffer[AMOUNT_NTC_THERMISTORS];
        temp_read_all(buffer);
        resume_Core1();

        for (uint8_t i = 0; i < AMOUNT_NTC_THERMISTORS; i++)
        {
            packet_dl.thermistor[i] = buffer[i];
        }

        packet_dl.thermistor[8] = analogReadTemp(ADC_REF);

        state = READ_OXY;
        break;
    }
    case READ_OXY:
    {
        pause_Core1();
        oxy_read_all(mesure_buffer);
        // delay(1000);
        resume_Core1();
        memcpy(packet_dl.oxy_measure, mesure_buffer, sizeof(mesure_buffer));
        state = READ_LIGHT;
        break;
    }
    case READ_LIGHT:
    {
        // float buffer[20];
        // light_read(buffer);
        // for (uint8_t i = 0; i < 6; i++)
        // {
        //     packet_dl.light[i] = buffer[i];
        // }
        packet_dl.pid[0] = pi_probe0.kp;
        packet_dl.pid[1] = pi_probe0.ki;
        packet_dl.pid[2] = pi_probe0.I_MAX;
        packet_dl.light[0] = pi_probe0.i_last;
        packet_dl.light[1] = pi_probe1.i_last;
        packet_dl.light[2] = pi_probe2.i_last;
        packet_dl.light[3] = pi_probe3.i_last;
        packet_dl.light[4] = pi_probe4.i_last;
        packet_dl.light[5] = pi_probe5.i_last;
        packet_dl.light[6] = pi_probe1.ki;
        packet_dl.light[7] = pi_probe1.kp;
        packet_dl.light[8] = pi_probe2.ki;
        packet_dl.light[9] = pi_probe2.kp;
        packet_dl.light[10] = pi_probe3.ki;
        packet_dl.light[11] = pi_probe3.kp;
        state = SAVESENDPACKET;
        break;
    }
    case SAVESENDPACKET:
    {
        sd_writestruct(&packet_dl, sd_filepath);
        tcp_send_packet(&packet_dl);
        state = CLEAR_PACKET;
        break;
    }
    case ERROR:
    {
        state = START;
        TCP_init = 0;
        sd_init = 0;
        light_init = 0;
        heat_init = 0;
    }
    default:
        state = ERROR;
        // error_handler(ERROR_STATE);
        debugf_error("State out of bounds\n");
        break;
    }
}

/**
 * Oxygen probe and Thermistors are both multiplexed.
 * The adress lines of those multiplexer are connected.
 * This methode is used to select a Oxygen Sensor or a Thermistor
 */
void select_oxy_or_ntc(const int ProbeorNTC)
{
    // debugf_status("select probe\n");

    /*initialise*/
    static uint8_t init = 0;
    if (!init)
    {
        pinMode(PIN_MUX_ADRES_0, OUTPUT);
        pinMode(PIN_MUX_ADRES_1, OUTPUT);
        pinMode(PIN_MUX_ADRES_2, OUTPUT);
        pinMode(PIN_MUX_OXY_DISABLE, OUTPUT);
        digitalWrite(PIN_MUX_OXY_DISABLE, 0);
        init = 1;
    }

    /*calculate pin mask*/
    uint8_t A0 = (ProbeorNTC - 1) & 0b00000001;
    uint8_t A1 = ((ProbeorNTC - 1) & 0b00000010) >> 1;
    uint8_t A2 = ((ProbeorNTC - 1) & 0b00000100) >> 2;

    /*setting mux pins*/
    digitalWrite(PIN_MUX_ADRES_0, A0);
    digitalWrite(PIN_MUX_ADRES_1, A1);
    digitalWrite(PIN_MUX_ADRES_2, A2);

    // debugf_info("Select S%u|A0:%u|A1:%u|A2:%u\n", ProbeorNTC, A0, A1, A2);
}

void state_print(unsigned int Status)
{
#if DEBUG_LEVEL >= 4
    switch (Status)
    {
    case START:
        debugf_status("<current state: START>\n");
        break;
    case CLEAR_PACKET:
        debugf_status("<current state: CLEAR_PACKET>\n");
        break;
    case READ_LIGHT:
        debugf_status("<current state: READ_LIGHT>\n");
        break;
    case READ_OXY:
        debugf_status("<current state: READ_OXY>\n");
        break;
    case READ_TEMP:
        debugf_status("<current state: READ_TEMP>\n");
        break;
    case SAVESENDPACKET:
        debugf_status("<current state: SAVESENDPACKET>\n");
        break;
    case ERROR:
        debugf_error("<current state: ERROR>\n");
        break;
    default:
        error_handler(ERROR_STATE);
        debugf_error("State out of bounds\n");
        break;
    }
#endif
}
