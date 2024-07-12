#include "header.h"
#include "debug_in_color.h"

static const char error_file_path[] = "Error_logging.bin";

const uint8_t ERROR_DESTINATION_NO_TCP = 1;
const uint8_t ERROR_DESTINATION_NO_SD = 2;
const uint8_t ERROR_DESTINATION_NO_TCP_SD = 3;

/**
 * saves/sends errors
 * @param destination: here y can specify where the Error code should end up.
 * leave it undefined to save it to sd and send it via the tcp connection.
 * ERROR_DESTINATION_NO_TCP, ERROR_DESTINATION_NO_SD and ERROR_DESTINATION_NO_TCP_SD are parameters too
 */
void error_handler(const unsigned int ErrorCode, const uint8_t destination)
{
    static char error_init = 0;

    // if (destination !=  ERROR_DESTINATION_NO_SD && destination != ERROR_DESTINATION_NO_TCP_SD)
    // {
    //     if (!error_init)
    //     {
    //         sd_writetofile("timestamp;errorcode", error_file_path);
    //         error_init = 1;
    //     }
    //     char string[200];
    //     snprintf(string, sizeof(string), "%u;%u", millis(), ErrorCode);
    //     sd_writetofile(string, error_file_path);
    // }

#if DEBUG_MODE == 2
    if (destination != ERROR_DESTINATION_NO_TCP && destination != ERROR_DESTINATION_NO_TCP_SD)
    {
        tpc_send_error((unsigned int)ErrorCode);
    }
#endif

#if DEBUG_MODE == 2

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