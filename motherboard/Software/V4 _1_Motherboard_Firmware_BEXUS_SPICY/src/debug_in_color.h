
#ifndef DEBUG_IN_COLOR_H
#define DEBUG_IN_COLOR_H
#include "header.h"
#include "debug_in_color.h"

#define LENGTHARRAY(array) ((sizeof(array)) / (sizeof(array[0])))

#ifndef DEBUG_LEVELS
#define DEBUG_LEVELS 4 /*changes the debug console prints. 0=just errors,1=Status Updates and sucess,2=Infos about running code,3 = Debug infos*/
#endif

/*-------Explanation----------------*/

// There are 4 debug levels:
/*
<= 0: no debug
> 0: just errors
> 1: Status Updates and sucess messages
> 2: Infos about running code
> 3: Debug infos
*/

// AThere are 2 debug modes:
/*
0 = no debug. Nothing gets printed anywhere
1 = Serial all debug messages & errors get printed on the Serial Termina
2 = TCP all debug messages get send to the specified TCP server.
Error debug messages are disabled to prevent infinite error loopings, but all not tcp errorcodes get downlinked regardless.
*/

/*-------Basic Debug -------------------*/
// replaces debugf() with a Serial output or a TPC downlink is actrivated
#if DEBUG_MODE == 0 // no debug
#define debug(...)
#define debugln(...)
#define debugf(...)
#define MESSURETIME_START
#define MESSURETIME_STOP
#endif

#if DEBUG_MODE == 1 // debug over Serial Console
#define debug(...) Serial.print(__VA_ARGS__)
#define debugln(...) Serial.println(__VA_ARGS__) /*Can be a Serial.println, a tcp downlink or disabled in the config*/
#define debugf(...) Serial.printf(__VA_ARGS__)   /*Can be a Serial.printf, a tcp downlink or disabled in the config*/
#define MESSURETIME_START        \
    unsigned long tb = 0;        \
    unsigned long ta = micros(); \
    debugf_magenta(".");
#define MESSURETIME_STOP \
    tb = micros();       \
    ta = tb - ta;        \
    debugf_magenta("Time: %.2fs|%.2fms|%ius\n", ta / 1000000.0, ta / 1000.0, ta);
#endif

#if DEBUG_MODE == 2 // debug over TCP packet
#define debug(...) tcp_sendf(__VA_ARGS__)
#define debugln(...) tcp_sendf(__VA_ARGS__) /*Can be a Serial.println, no tcp downlink or disabled in the config*/
#define debugf(...) tcp_sendf(__VA_ARGS__)  /*Can be a Serial.printf, a tcp downlink or disabled in the config*/
#define MESSURETIME_START
#define MESSURETIME_STOP
#endif

/*-------Debug levels------------*/
#ifdef DEBUG_LEVELS
#if DEBUG_LEVELS > 0 && DEBUG_MODE != 2
/*prints errors in red*/
#define debugf_error(...) debugf_red(__VA_ARGS__)
#else
#define debugf_error(...)
#endif
#endif

#if DEBUG_LEVELS > 1
/*prints in white and is used for not breaking warning abut running processes*/
#define debugf_warn(...) debugf_yellow(__VA_ARGS__)
#else
#define debugf_warn(...)
#endif

#if DEBUG_LEVELS > 2
/*prints in yellow gives status about what is running right now*/
#define debugf_status(...) \
    debugf_magenta(">")    \
        debugf_magenta(__VA_ARGS__)

#define debugf_sucess(...) debugf_green(__VA_ARGS__)
#else
#define debugf_status(...)
#define debugf_sucess(...)
#endif

#if DEBUG_LEVELS > 3
/*prints in white and is used for non critical infos abut running processes*/
#define debugf_info(...) debugf_white(__VA_ARGS__)
#else
#define debugf_info(...)
#endif

/*---------Error handling------------------*/
/**
 * saves/sends errors
 * @param destination: here y can specify where the Error code should end up.
 * leave it undefined to save it to sd and send it via the tcp connection.
 * ERROR_DESTINATION_NO_TCP, ERROR_DESTINATION_NO_SD and ERROR_DESTINATION_NO_TCP_SD are parameters too
 */
void error_handler(const unsigned int ErrorCode, int destination)
{
    /*initialising SD card*/
    static char error_init = 0;
    static char error_log_file_path[100];

    if (!error_init)
    {
        snprintf(error_log_file_path, 99, "error_log[%u].csv", nMOTHERBOARD_BOOTUPS);
        sd_writetofile("timestamp;errorcode", error_log_file_path);
        error_init = 1;
    }

    /*saves error to SD card*/
    if (destination != ERROR_DESTINATION_NO_SD && destination != ERROR_DESTINATION_NO_TCP_SD)
    /*if saving to sd card is okay*/
    {
        char string[200];
        snprintf(string, sizeof(string), "%u;%u", millis(), ErrorCode);
        sd_writetofile(string, error_log_file_path);
    }

    /*sends error via TCP*/
#if DEBUG_MODE == 2
    if (destination != ERROR_DESTINATION_NO_TCP && destination != ERROR_DESTINATION_NO_TCP_SD)
    /*if sending via tcp is okay*/
    {
        tpc_send_error((unsigned int)ErrorCode);
    }
#endif
}

/*-------Serial printing colours------------*/
// provides coloured versions of debugf like debugf_yellow()
#define debugf_yellow(...) \
    debugf("\033[33m");    \
    debugf("\033[1m");     \
    debugf(__VA_ARGS__);   \
    debugf("\033[0m");

#define debugf_black(...) \
    debugf("\033[30m");   \
    debugf(__VA_ARGS__);  \
    debugf("\033[0m");

#define debugf_red(...)  \
    debugf("\033[31m");  \
    debugf("\033[7m");   \
    debugf(__VA_ARGS__); \
    debugf("\033[0m");

#define debugf_green(...) \
    debugf("\033[32m");   \
    debugf(__VA_ARGS__);  \
    debugf("\033[0m");

#define debugf_blue(...) \
    debugf("\033[34m");  \
    debugf(__VA_ARGS__); \
    debugf("\033[0m");

#define debugf_magenta(...) \
    debugf("\033[35m");     \
    debugf(__VA_ARGS__);    \
    debugf("\033[0m");

#define debugf_cyan(...) \
    debugf("\033[36m");  \
    debugf(__VA_ARGS__); \
    debugf("\033[0m");

#define debugf_white(...) \
    debugf("\033[37m");   \
    debugf(__VA_ARGS__);  \
    debugf("\033[0m");

#endif