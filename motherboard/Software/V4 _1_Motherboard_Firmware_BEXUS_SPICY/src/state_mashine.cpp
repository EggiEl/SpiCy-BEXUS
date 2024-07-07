#include "header.h"

void state_print(unsigned int Status);
void resetState();

enum states
{
    START,
    CREATE_PACKET,
    READ_TEMP,
    READ_OXY,
    READ_LIGHT,
    SAVESENDPACKET,
    ERROR
};

enum states state = START;

struct packet *packet_dl = NULL; /*Pointer to the packet wich will be the next downlink*/
struct oxy_mesure oxy_mesure_buff;
/*
 * Changes the current state to the next one
 */
void nextState()
{
    const char sd_filepath[] = "data.bin";
    static float buffer[20];
    switch (state)
    {
    case START:
    {
        state_print(START);
        state = CREATE_PACKET;
        break;
    }
    case CREATE_PACKET:
    {
        state_print(CREATE_PACKET);
        destroy_packet(packet_dl);
        packet_dl = packet_create();
        if (packet_dl)
        {
            state = READ_TEMP;
        }
        else
        {
            debugf_error("state malloc nex packages failed");
            state = ERROR;
        }
        break;
    }
    case READ_TEMP:
    {
        state_print(READ_TEMP);

        temp_read_all(buffer);
        for (uint8_t i = 0; i < 8; i++)
        {
            packet_dl->thermistor[i] = buffer[i];
        }
        packet_dl->thermistor[8] = analogReadTemp(ADC_REF);

        state = READ_OXY;
        break;
    }
    case READ_OXY:
    {
        state_print(READ_OXY);
        oxy_read_all(&oxy_mesure_buff);
        for (uint8_t i = 0; i < 6; i++)
        {
            packet_dl->pyro_temp[i] = oxy_mesure_buff.pyro_temp[i];
            packet_dl->pyro_pressure[i] = oxy_mesure_buff.pyro_pressure[i];
            packet_dl->pyro_oxy[i] = oxy_mesure_buff.pyro_oxy[i];
        }

        state = READ_LIGHT;
        break;
    }
    case READ_LIGHT:
    {
        state_print(READ_LIGHT);
        // light_read(buffer);
        for (uint8_t i = 0; i < 6; i++)
        {
            packet_dl->light[i] = buffer[i];
        }
        state = SAVESENDPACKET;
        break;
    }
    case SAVESENDPACKET:
    {
        state_print(SAVESENDPACKET);
        sd_writestruct(packet_dl, sd_filepath);
        tcp_send_packet(packet_dl);
        state = CREATE_PACKET;
        break;
    }
    case ERROR:
    {
        state_print(ERROR);
        resetState();
    }
    default:
        state = ERROR;
        break;
    }
}

void resetState()
{
    destroy_packet(packet_dl);
    TCP_init = 0;
    sd_init = 0;
    state = START;
}

void state_print(unsigned int Status)
{
    switch (Status)
    {
    case START:
        debugf_status("<current state: START>\n");
        break;
    case CREATE_PACKET:
        debugf_status("<current state: CREATE_PACKET>\n");
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
        debugf_error("State out of bounds\n");
        break;
    }
}