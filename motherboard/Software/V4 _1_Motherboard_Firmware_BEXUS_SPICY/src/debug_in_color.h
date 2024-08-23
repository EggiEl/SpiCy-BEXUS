
#ifndef DEBUG_IN_COLOR_H
#define DEBUG_IN_COLOR_H

#include "config.h"
#include <Arduino.h>

#define LENGTHARRAY(array) ((sizeof(array)) / (sizeof(array[0])))

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 4 /*changes the debug console prints. 0=just errors,1=Status Updates and sucess,2=Infos about running code,3 = Debug infos*/
#endif

/*-------Explanation----------------*/

// There are 4 debug levels:
/*
<= 0: no debug
> 0: just errors
> 1: Status Updates and sucess messages
> 2: Infos about running code
> 3: Debug infos
> 4: printing current state
*/

// There are 2 debug modes:
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
#define debug(...)
#define debugln(...)                          /*Can be a Serial.println, no tcp downlink or disabled in the config*/
#define debugf(...) tcp_sendf(__VA_ARGS__) /*Can be a Serial.printf, a tcp downlink or disabled in the config*/
#define MESSURETIME_START
#define MESSURETIME_STOP
#endif

/*-------Debug levels------------*/
#ifdef DEBUG_LEVEL
#if DEBUG_LEVEL >= 0 && DEBUG_MODE != 2
/*prints errors in red*/
#define debugf_error(...) debugf_red(__VA_ARGS__)
#else
#define debugf_error(...)
#endif
#endif

#if DEBUG_LEVEL >= 1
/*prints in white and is used for not breaking warning abut running processes*/
#define debugf_warn(...) debugf_yellow(__VA_ARGS__)
#else
#define debugf_warn(...)
#endif

#if DEBUG_LEVEL >= 2
/*prints in yellow gives status about what is running right now*/
#define debugf_status(...) \
    debugf_magenta(">")    \
        debugf_magenta(__VA_ARGS__)

#define debugf_sucess(...) debugf_green(__VA_ARGS__)
#else
#define debugf_status(...)
#define debugf_sucess(...)
#endif

#if DEBUG_LEVEL >= 3
/*prints in white and is used for non critical infos abut running processes*/
#define debugf_info(...) debugf_white(__VA_ARGS__)
#else
#define debugf_info(...)
#endif

#if DEBUG_LEVEL >= 4
/*print the current state of the statemashine running on the first core*/
#endif

/*-------Serial printing colours------------*/
// provides coloured versions of debugf like debugf_yellow()
#if DEBUG_COLOUR == 1
#define debugf_yellow(...) \
    debugf("\033[33m");    \
    debugf(__VA_ARGS__);   \
    debugf("\033[0m");

#define debugf_black(...) \
    debugf("\033[30m");   \
    debugf(__VA_ARGS__);  \
    debugf("\033[0m");

#define debugf_red(...)  \
    debugf("\033[31m");  \
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
#else
#define debugf_yellow(...) debugf(__VA_ARGS__)
#define debugf_black(...) debugf(__VA_ARGS__)
#define debugf_red(...) debugf(__VA_ARGS__)
#define debugf_green(...) debugf(__VA_ARGS__)
#define debugf_blue(...) debugf(__VA_ARGS__)
#define debugf_magenta(...) debugf(__VA_ARGS__)
#define debugf_cyan(...) debugf(__VA_ARGS__)
#define debugf_white(...) debugf(__VA_ARGS__)
#endif

#endif
