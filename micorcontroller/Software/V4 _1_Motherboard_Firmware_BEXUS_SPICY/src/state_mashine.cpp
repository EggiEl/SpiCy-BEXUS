#include "header.h"

void state_print(int Status);
void resetState();

static enum states {
    START,
    CREATE_PACKET,
    READ_TEMP,
    READ_OXY,
    READ_LIGHT,
    SAVESENDPACKET,
    ERROR
} state;

struct packet *packet_dl = NULL; /*Pointer to the packet wich will be the next downlink*/
/*
 * Changes the current state to the next one
 */
void nextState()
{
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
        float Temp_buf[8];
        temp_read_all(Temp_buf);
        for (uint8_t i = 0; i < 6; i++)
        {
            packet_dl->thermistor[i] = Temp_buf[i];
        }
        state = READ_OXY;
        break;
    }
    case READ_OXY:
    {
        state_print(READ_OXY);
        float *Oxy_buf = oxy_read_all();
        for (uint8_t i = 0; i < 6; i++)
        {
            packet_dl->pyro_temp[i] = Oxy_buf[i];
        }
        free(Oxy_buf);
        state = READ_LIGHT;
        break;
    }
    case READ_LIGHT:
    {
        state_print(READ_LIGHT);
        float buffer[7];
        light_read(buffer, 0);
        state = SAVESENDPACKET;
        break;
    }
    case SAVESENDPACKET:
    {
        state_print(SAVESENDPACKET);
        // sd_writestruct(packet_dl, "oi");
        char success = tcp_send_packet(packet_dl);
        destroy_packet(packet_dl);
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

void state_print(int Status)
{
    switch (Status)
    {
    case START:
        debugf_status("<current state: START>\n");
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
        debugf_error("<current state: SAVESENDPACKET>\n");
        break;
    case ERROR:
        debugf_error("<current state: ERROR>\n");
        break;

    default:
        debugf_error("State out of bounds\n");
        break;
    }
}