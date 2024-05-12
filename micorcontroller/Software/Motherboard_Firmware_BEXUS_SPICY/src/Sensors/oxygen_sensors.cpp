#include "header.h"

float *oxygen_readall()
{
    float *Oxy_buf = (float *)malloc(6 * sizeof(float));
    for (uint8_t i = 0; i < 6; i++)
    {
        Oxy_buf[i] = i * 100;
    }
    return Oxy_buf;
}
