#include "header.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);

char oxy_init = 0;
void oxy_setup()
{
    mySerial.begin(19200);
    if(millis()<(3*60*1000)){
        debugf_warn("Oxygen Sensors warnuptime of 3min isnt fullfilled yet\n");
    }
}

void oxy_readwrite()
{
    if (mySerial.available())
        Serial.write(mySerial.read());

    if (Serial.available())
        mySerial.write(Serial.read());
}

/**/
float *oxy_readall()
{
    float *Oxy_buf = (float *)malloc(6 * sizeof(float));
    for (uint8_t i = 0; i < 6; i++)
    {
        Oxy_buf[i] = i * 100;
    }
    delay(2000);
    return Oxy_buf;
}

char OXY_OPTICALCHANNEL = 1;
char OXY_SENSORSENABLED = 47; // This parameter defines the enabled sensor types bit for bit
float oxy_readone(uint8_t number)
{
    /*boots up sensor*/
    oxy_commandhandler(2, "MEA", OXY_OPTICALCHANNEL, OXY_SENSORSENABLED);

    return 1.0;
}

/**
 * Calibrate oxygen Sensor at ambient air
*@param channel Optical channel number. Set C=1
*@param temp Temperature of the calibration standard in C
*@param pressure Ambient air pressure in mbar
*@param humidity Relative humidity of the ambient air in units of 10-3 %RH (e.g. 50000
means 50%RH) Set H=100000 (=100%RH) for calibrations in air saturated
water
 **/
void oxy_calibrateSensoratair(uint8_t channel, float temp, float pressure, float humidity)
{
    unsigned int temp_out = temp * 1000.0;
    unsigned int pressure_out = pressure * 1000.0;
    unsigned int humidity_out = humidity * 1000.0;
    oxy_commandhandler(4, "CHI", channel, temp_out, pressure_out, humidity_out);
}

/**
 * Calibrate oxygen Sensor at 0* Oxy
*@param channel Optical channel number. Set C=1
*@param temp Temperature of the calibration standard in C
 **/
void oxy_calibrateSensoratair(uint8_t channel, float temp)
{
    unsigned int temp_out = temp * 1000.0;
    oxy_commandhandler(2, "CLO", channel, temp_out);
}

/*save config in flash*/
void oxy_saveconfig(uint8_t channel)
{
    oxy_commandhandler(0, "SVS", channel);
}

/*prints info abouth*/
void oxy_info(uint8_t channel)
{
    oxy_commandhandler(0, "VERS", channel);
    /*recieve info*/
}

/**
 * Sends a command via SoftwareSerial to a Cxigen Sensor
 * @param nParam defines how may Parameters y wanna give the fuction. For the command MSP 1000 for example nParam = 1
 */
char oxy_commandhandler(uint8_t nParam, const char * Command, uint8_t channel, char param0, char param1, char param2, char param3)
{
    /*Sending Command*/
    if (nParam > 4)
    {
        debugf_warn("oxy_Send too many command param\n");
    }

    mySerial.write(Command);
    if (nParam == 0)
    {
        mySerial.write("\r");
        return 1;
    }
    if (nParam > 0)
    {
        mySerial.write(" ");
        mySerial.write(param0);
    }
    else
    {
        mySerial.write("\r");
        return 1;
    }
    if (nParam > 1)
    {
        mySerial.write(" ");
        mySerial.write(param1);
    }
    else
    {
        mySerial.write("\r");
        return 1;
    }
    if (nParam > 2)
    {
        mySerial.write(" ");
        mySerial.write(param2);
    }
    else
    {
        mySerial.write("\r");
        return 1;
    }
    if (nParam > 3)
    {
        mySerial.write(" ");
        mySerial.write(param3);
    }
    else
    {
        mySerial.write("\r");
        return 1;
    }
     return 1;
}


void handleError(int errorCode)
{
    switch (errorCode)
    {
    case -1:
        debugf_error("General: A non-specific error occurred.\n");
        break;
    case -2:
        debugf_error("Channel: The requested optical channel does not exist.\n");
        break;
    case -11:
        debugf_error("Memory Access: Memory access violation either caused by a non-existing requested register, or by an out-of-range address of the requested value.\n");
        break;
    case -12:
        debugf_error("Memory Lock: The requested memory is locked (system register) and a write access was requested.\n");
        break;
    case -13:
        debugf_error("Memory Flash: An error occurred while saving the registers permanently. The SVS request should be repeated to ensure a correct permanent memory.\n");
        break;
    case -14:
        debugf_error("Memory Erase: An error occurred while erasing the permanent memory region for the registers. The SVS request should be repeated.\n");
        break;
    case -15:
        debugf_error("Memory Inconsistent: The registers in RAM are inconsistent with the permanently stored registers after processing SVS. The SVS request should be repeated.\n");
        break;
    case -21:
        debugf_error("UART Parse: An error occurred while parsing the command string. The last command should be repeated.\n");
        break;
    case -22:
        debugf_error("UART Rx: The command string was not received correctly (e.g., device was not ready, last request was not terminated correctly). Repeat the last command.\n");
        break;
    case -23:
        debugf_error("UART Header: The command header could not be interpreted correctly (must contain only characters from A-Z). Repeat the last command.\n");
        break;
    case -24:
        debugf_error("UART Overflow: The command string could not be processed fast enough to prevent an overflow of the internal receiving buffer.\n");
        break;
    case -25:
        debugf_error("UART Baudrate: The requested baudrate is not supported. No baudrate change took place.\n");
        break;
    case -26:
        debugf_error("UART Request: The command header does not match any of the supported commands.\n");
        break;
    case -27:
        debugf_error("UART Start Rx: The device was waiting for incoming data; however, the next event was not triggered by receiving a command.\n");
        break;
    case -28:
        debugf_error("UART Range: One or more parameters of the command are out of range.\n");
        break;
    case -30:
        debugf_error("I2C Transfer: There was an error transferring data on the I2C bus.\n");
        break;
    case -40:
        debugf_error("Temp Ext: The communication with the sample temperature sensor was not successful.\n");
        break;
    case -41:
        debugf_error("Periphery No Power: The power supply of the device periphery (sensors, SD card) is not switched on.\n");
        break;
    default:
        debugf_error("Unknown error code.\n");
        break;
    }
}
