#include "header.h"

static enum states
{
    START,
    CREATE_PACKET,
    READ_TEMP,
    READ_OXY,
    READ_LIGHT,
    SAVESENDPACKET,
    ERROR
} state;

struct packet *packet_dl; /*Pointer to the packet wich will be the next downlink*/
/*
 * Changes the current state to the next one
 */
void nextState()
{
    switch (state)
    {
    case START:
    {
        packet_dl = NULL;
        state = CREATE_PACKET;
        break;
    }
    case CREATE_PACKET:
    {
        packet_dl = packet_create();
        if (packet_dl)
        {
            state = READ_TEMP;
        }
        else
        {
            state = START;
        }
        break;
    }
    case READ_TEMP:
    {
        float *Temp_buf = temp_read_cable();
        for (uint8_t i = 0; i < 6; i++)
        {
            packet_dl->tempTube[i] = Temp_buf[i];
        }
        free(Temp_buf);
        state = READ_OXY;
        break;
    }
    case READ_OXY:
    {
        float *Oxy_buf = oxy_readall();
        for (uint8_t i = 0; i < 6; i++)
        {
            packet_dl->oxigen[i] = Oxy_buf[i];
        }
        free(Oxy_buf);
        state = READ_LIGHT;
        break;
    }
    case READ_LIGHT:
    {
        state = SAVESENDPACKET;
        break;
    }
    case SAVESENDPACKET:
    {
        // sd_writestruct(packet_dl, "oi");
        char success = tcp_send_packet(packet_dl);
        free(packet_dl);
        state = START;
        break;
    }
    case ERROR:
    {
        TCP_init = 0;
    	sd_init = 0;
        free(packet_dl);
        state = START;
    }
    default:
        state = ERROR;
        break;
    }
}

void resetState()
{
    state = START;
}
