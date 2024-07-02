#include "header.h"

void error_handler(unsigned int ErrorCode){
    #if DEBUG == 2
    tpc_send_error((unsigned int)ErrorCode);
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