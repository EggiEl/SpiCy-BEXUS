#include "header.h"
#include <stdio.h>

/**
 * saves/sends errors
 * @param destination: here y can specify where the Error code should end up.
 * leave it undefined to save it to sd and send it via the tcp connection.
 * ERROR_DESTINATION_NO_TCP, ERROR_DESTINATION_NO_SD and ERROR_DESTINATION_NO_TCP_SD are parameters too
 */
void error_handler(const int ErrorCode, int destination)
{
    /*saves error to SD card*/
    if (destination != ERROR_DESTINATION_NO_SD && destination != ERROR_DESTINATION_NO_TCP_SD)
    /*if saving to sd card is okay*/
    {
        /*initialising SD card*/
        static char error_sd_init = 0;
        static char error_log_file_path[100];

        if (!error_sd_init)
        {
            snprintf(error_log_file_path, 99, "error_log[%lu].csv", nMOTHERBOARD_BOOTUPS);
            sd_writetofile("timestamp;errorcode", error_log_file_path);
            error_sd_init = 1;
        }

        char string[200];
        snprintf(string, sizeof(string), "%u;%u", millis(), ErrorCode);
        sd_writetofile(string, error_log_file_path);
    }

    /*sends error via TCP*/
#if DEBUG_MODE == 2
    if (destination != ERROR_DESTINATION_NO_TCP && destination != ERROR_DESTINATION_NO_TCP_SD)
    /*if sending via tcp is okay*/
    {
        tpc_send_error((unsigned char)ErrorCode);
    }
#endif
}
