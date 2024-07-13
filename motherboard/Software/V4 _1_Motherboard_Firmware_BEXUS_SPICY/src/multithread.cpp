#include "header.h"
#include "debug_in_color.h"

uint8_t flag_pause_core1 = 0;     // core 0 can raise this flag to pause core 1
uint8_t flag_core1_isrunning = 1; // states whether core1 is running at a given moment

void pause_Core1()
{
    flag_pause_core1 = 1;
    while (flag_core1_isrunning)
    {
    }
    debugf_status("core 1 puased\n");
}

void resume_Core1()
{
    debugf_status("core 1 resumed\n");
    flag_pause_core1 = 0;
}
