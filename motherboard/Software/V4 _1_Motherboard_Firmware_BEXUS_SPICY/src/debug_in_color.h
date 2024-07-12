
#ifndef DEBUG_IN_COLOR_H
#define DEBUG_IN_COLOR_H
#include "header.h"
#include "debug_in_color.h"

#define LENGTHARRAY(array) ((sizeof(array)) / (sizeof(array[0])))

#ifndef COLOUR_SERIAL
#define COLOUR_SERIAL 1 /*activates Serial.printing with color*/
#endif

#ifndef DEBUG_LEVELS
#define DEBUG_LEVELS 4 /*changes the debug console prints. 0=just errors,1=Status Updates and sucess,2=Infos about running code,3 = Debug infos*/
#endif

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
#define debug(...) Serial.print(__VA_ARGS__)
#define debugln(...) Serial.println(__VA_ARGS__) /*Can be a Serial.println, a tcp downlink or disabled in the config*/
#define debugf(...) Serial.printf(__VA_ARGS__)   /*Can be a Serial.printf, a tcp downlink or disabled in the config*/
#define MESSURETIME_START
#define MESSURETIME_STOP
#endif

/*-------Serial printing colours------------*/
// provides coloured versions of debugf like debugf_yellow()
#if COLOUR_SERIAL == 1
#define debugf_yellow(...) \
    debug("\033[33m");     \
    debug("\033[1m");      \
    debugf(__VA_ARGS__);   \
    debug("\033[0m");

#define debugf_black(...) \
    debug("\033[30m");    \
    debugf(__VA_ARGS__);  \
    debug("\033[0m");

#define debugf_red(...)  \
    debug("\033[31m");   \
    debug("\033[7m");    \
    debugf(__VA_ARGS__); \
    debug("\033[0m");

#define debugf_green(...) \
    debug("\033[32m");    \
    debugf(__VA_ARGS__);  \
    debug("\033[0m");

#define debugf_blue(...) \
    debug("\033[34m");   \
    debugf(__VA_ARGS__); \
    debug("\033[0m");

#define debugf_magenta(...) \
    debug("\033[35m");      \
    debugf(__VA_ARGS__);    \
    debug("\033[0m");

#define debugf_cyan(...) \
    debug("\033[36m");   \
    debugf(__VA_ARGS__); \
    debug("\033[0m");

#define debugf_white(...) \
    debug("\033[37m");    \
    debugf(__VA_ARGS__);  \
    debug("\033[0m");

// You can keep the original definitions for color setting macros without changes.
#define SET_COLOUR_BLACK debug("\033[30m");
#define SET_COLOUR_RED debug("\033[31m");
#define SET_COLOUR_GREEN debug("\033[32m");
#define SET_COLOUR_YELLOW debug("\033[33m");
#define SET_COLOUR_BLUE debug("\033[34m");
#define SET_COLOUR_MAGENTA debug("\033[35m");
#define SET_COLOUR_CYAN debug("\033[36m");
#define SET_COLOUR_WHITE debug("\033[37m");
#define SET_COLOUR_RESET debug("\033[0m");
#else
#define debugf_yellow(...) debugf(__VA_ARGS__)
#define debugf_black(...) debugf(__VA_ARGS__)
#define debugf_red(...) debugf(__VA_ARGS__)
#define debugf_green(...) debugf(__VA_ARGS__)
#define debugf_blue(...) debugf(__VA_ARGS__)
#define debugf_magenta(...) debugf(__VA_ARGS__)
#define debugf_cyan(...) debugf(__VA_ARGS__)
#define debugf_white(...) debugf(__VA_ARGS__)

#define SET_COLOUR_BLACK
#define SET_COLOUR_RED
#define SET_COLOUR_GREEN
#define SET_COLOUR_YELLOW
#define SET_COLOUR_BLUE
#define SET_COLOUR_MAGENTA
#define SET_COLOUR_CYAN
#define SET_COLOUR_WHITE
#define SET_COLOUR_RESET
#endif

/*-------Debug levels------------*/
#ifdef DEBUG_LEVELS
#if DEBUG_LEVELS > 3
/*prints in white and is used for non critical infos abut running processes*/
#define debugf_info(...) debugf_white(__VA_ARGS__)
#else
#define debugf_info(...)
#endif

#if DEBUG_LEVELS > 2
/*prints in yellow gives status about what is running right now*/
#define debugf_status(...)      \
        debugf_magenta(">") \
        debugf_magenta(__VA_ARGS__)

#define debugf_sucess(...) debugf_green(__VA_ARGS__)
#else
#define debugf_status(...)
#define debugf_sucess(...)
#endif

#if DEBUG_LEVELS > 1
/*prints in white and is used for not breaking warning abut running processes*/
#define debugf_warn(...) debugf_yellow(__VA_ARGS__)
#else
#define debugf_warn(...)
#endif

#if DEBUG_LEVELS > 0
/*prints errors in red*/
#define debugf_error(...) debugf_red(__VA_ARGS__)
#else
#define debugf_error(...)
#endif
#endif

/*-----ERROR Handling------------*/

#endif