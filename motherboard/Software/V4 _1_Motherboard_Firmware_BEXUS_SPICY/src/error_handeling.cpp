#include "header.h"

static const char error_file_path[] = "Error_logging.bin";

void error_handler(unsigned int ErrorCode)
{
    static char error_init = 0;

    if (ErrorCode != ERROR_SD_INI)
    {
        if (!error_init)
        {
            sd_writetofile("timestamp;errorcode", error_file_path);
        }
        char string[200];
        snprintf(string, sizeof(string), "%u;%u", millis(), ErrorCode);
        sd_writetofile(string, error_file_path);
    }
    
#if DEBUG == 2
    if (ErrorCode != ERROR_TCP_INI)
    {
        tpc_send_error((unsigned int)ErrorCode);
    }
#endif

#if DEBUG == 2

    switch (ErrorCode)
    {
    case ERROR_SD_INI:
        debugf_error("sd init failed\n");
        break;

    default:
        break;
    }
#endif
}