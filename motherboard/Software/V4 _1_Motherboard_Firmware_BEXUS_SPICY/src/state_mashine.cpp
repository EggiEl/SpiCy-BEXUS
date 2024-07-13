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
void nextState()
{
    static enum states state = START;
    static struct packet packet_dl = {0}; /*packet wich will be the next downlink*/
    static struct OxygenReadout mesure_buffer[6];

    const char sd_filepath[] = "data.bin";
    static float buffer[20];
    state_print(state);
    switch (state)
    {
    case START:
    {
        state = CLEAR_PACKET;
        break;
    }
    case CLEAR_PACKET:
    {
        state = READ_TEMP;
        struct packet *new_packet = packet_create();
        memcpy(&packet_dl, new_packet, sizeof(struct packet));
        free_ifnotnull(new_packet);
        break;
    }
    case READ_TEMP:
    {
        pause_Core1();
        temp_read_all(buffer);
        resume_Core1();
        for (uint8_t i = 0; i < 8; i++)
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
        resume_Core1();
        memcpy(packet_dl.oxy_measure, mesure_buffer, sizeof(mesure_buffer));
        state = READ_LIGHT;
        break;
    }
    case READ_LIGHT:
    {
        delay(1000);
        // light_read(buffer, 0);
        // for (uint8_t i = 0; i < 6; i++)
        // {
        //     packet_dl.light[i] = buffer[i];
        // }
        state = SAVESENDPACKET;
        break;
    }
    case SAVESENDPACKET:
    {
        // sd_writestruct(&packet_dl, sd_filepath);
        // tcp_send_packet(packet_dl);
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
        break;
    }
}

void state_print(unsigned int Status)

{
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
}

void select_probe_or_NTC(const uint8_t ProbeorNTC)
{
    // debugf_status("select probe\n");
    static uint8_t init = 0;
    if (!init)
    {
        pinMode(PIN_PROBEMUX_0, OUTPUT);
        pinMode(PIN_PROBEMUX_1, OUTPUT);
        pinMode(PIN_PROBEMUX_2, OUTPUT);
        pinMode(PIN_MUX_OXY_DISABLE, OUTPUT);
        digitalWrite(PIN_MUX_OXY_DISABLE, 0);
        init = 1;
    }

    uint8_t A0 = (ProbeorNTC - 1) & 0b00000001;
    uint8_t A1 = ((ProbeorNTC - 1) & 0b00000010) >> 1;
    uint8_t A2 = ((ProbeorNTC - 1) & 0b00000100) >> 2;

    digitalWrite(PIN_PROBEMUX_0, A0);
    digitalWrite(PIN_PROBEMUX_1, A1);
    digitalWrite(PIN_PROBEMUX_2, A2);
    // debugf_info("Select S%u|A0:%u|A1:%u|A2:%u\n", ProbeorNTC, A0, A1, A2);
}