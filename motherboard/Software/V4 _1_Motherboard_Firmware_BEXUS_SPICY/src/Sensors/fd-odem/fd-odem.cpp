#include "header.h"
#include "SoftwareSerial.h"

SoftwareSerial Oxy_SoftwareSerial(PIN_OX_RX, PIN_OX_TX); // https://arduino-pico.readthedocs.io/en/latest/serial.html
#define oxySerial Serial1
char oxy_init = 0;
char oxy_calib = 0;

float oxy_meassure(const uint8_t number);
void oxy_decode_mesurement_errors(const u32_t R0);
void oxy_decode_general_error(const char errorCode_buff[]);
struct OxygenReadout
{
    s32_t error = 0;
    s32_t dphi = 0;
    s32_t umolar = 0;
    s32_t mbar = 0;
    s32_t airSat = 0;
    s32_t tempSample = 0;
    s32_t tempCase = 0;
    s32_t signalIntensity = 0;
    s32_t ambientLight = 0;
    s32_t pressure = 0;
    s32_t resistorTemp = 0;
    s32_t percentOtwo = 0;
};

/*setups the SoftwareSerial Connection to the oxygen sensor*/
void oxy_setup()
{
    debugf_status("<oxy_setup>\n");
    if (millis() < (3 * 60 * 1000))
    {
        debugf_warn("Oxygen Sensors warnuptime of 3min isnt fullfilled yet\n");
    }
    // enable USART 0
    pinMode(PIN_PROBEMUX_0, OUTPUT);
    pinMode(PIN_PROBEMUX_1, OUTPUT);
    pinMode(PIN_PROBEMUX_2, OUTPUT);
    pinMode(PIN_MUX_OXY_DISABLE, OUTPUT);

    digitalWrite(PIN_MUX_OXY_DISABLE, 0);
    digitalWrite(PIN_PROBEMUX_0, 0);
    digitalWrite(PIN_PROBEMUX_1, 0);
    digitalWrite(PIN_PROBEMUX_2, 0);

#if oxySerial == Serial1
    oxySerial.setTX(PIN_OX_TX);
    oxySerial.setRX(PIN_OX_RX);
// oxySerial.setPollingMode(true);
// oxySerial.setFIFOSize(128);
#endif

    oxySerial.begin(OXY_BAUD);

    oxySerial.write("#LOGO\r", 6);
    oxySerial.flush();
    char buffer[20];
    oxySerial.readBytesUntil('\r', buffer, sizeof(buffer));

    debugf_sucess("setup was succesfull\n");
}

bool oxy_isconnected()
{
    char buffer[20];
    oxySerial.write("#LOGO\r", 6);
    oxySerial.flush();
    /*reads out return values*/
    oxySerial.readBytesUntil('\r', buffer, sizeof(buffer));
    if (buffer[0] == '#' && buffer[1] == 'L' && buffer[2] == 'O' && buffer[3] == 'G' && buffer[4] == 'O')
    {
        return 1;
    }
    else
    {
        debugf_warn("Return for testing command #LOGO:\"%s\"\n", buffer);
        oxy_decode_general_error(buffer);
        return 0;
    }
}

/*Starts Console for talk with fd-odem module*/
void oxy_console()
{
    debugf_status("Console for Pyrosience FD-OEM Module\n");
    debugf_info("/q to exit.\n");

    if (!oxy_init)
    {
        oxy_setup();
    }
    // oxySerial.listen();

    oxy_meassure(1);
    // oxy_commandhandler("#LOGO");

    while (1)
    {
        /*recives & prints data from FD-ODEM*/
        if (oxySerial.available())
        {
            Serial.write(oxySerial.read());
        }

        /*sends data to FD-ODEM*/
        if (Serial.available() > 4)
        {
            char *buffer = (char *)calloc(1, COMMAND_LENGTH_MAX + 1);
            Serial.readBytesUntil('\n', buffer, COMMAND_LENGTH_MAX);
            buffer[COMMAND_LENGTH_MAX] = '\0';

            if (buffer[0] == '/' && buffer[1] == 'q')
            {
                break;
            }
            oxy_commandhandler(buffer, RETURN_LENGTH_MAX);
            free_ifnotnull(buffer);
        }

        /*resets Watchdog*/
        rp2040.wdt_reset();
    }
    debugf_status("closed FD-OEM console\n");
}

/**
 * Sends a command via SoftwareSerial to a Oxigen Sensor
 * please check befor calling if oxy_init is set
 * @param Command Command and all parameters as a string. dosent matter is y add "/r", works with and without
 * @param nReturn how many return bytes y got as an answer of the dam thing. Do big dosen´t matter, to small is bad.
 * If set to 0 the max return string length is set.
 * @return success: returns full reply string, error: returns NULL and gives Error the ErrorHandling() function
 */
char *oxy_commandhandler(const char command[], uint8_t nReturn)
{

    /*creates return buffer*/
    if (nReturn == 0)
    {
        nReturn = RETURN_LENGTH_MAX;
    }
    char *buffer = (char *)calloc(nReturn, sizeof(char));
    if (!buffer)
    {
        debugf_error("oxy commandhandler memory allocation fail\n");
        return NULL;
    }

    /*Sending Command*/
    debugf_status("sending oxy_command:%s\n", command);
    unsigned int command_length = strlen(command);
    if (command[command_length - 1] == '\r') // makes sure that the command is terminated
    {
        oxySerial.write(command, command_length);
    }
    else
    {
        oxySerial.write(command, command_length);
        oxySerial.write('\r');
    }

    /*flushes output*/
    oxySerial.flush();

    /*reads out return values*/
    unsigned int recievedbytes = oxySerial.readBytesUntil('\r', buffer, nReturn);

    /*checks if still data avaliable*/
    if (oxySerial.available())
    {
        debugf_warn("OxyComandHandler: There are more than nReturn %i bytes for command %s\n", nReturn, command);
    }

    uint8_t success = 1;
    /*checks for errors*/
    if (buffer[0] == '#' &&
        buffer[1] == 'E' &&
        buffer[2] == 'R' &&
        buffer[3] == 'R' &&
        buffer[4] == 'O')
    {
        oxy_decode_general_error(buffer);
        success = 0;
    }

    if (recievedbytes == 0)
    {
        debugf_warn("no answer from oxy sensor\n");
        success = 0;
    }

    if (success)
    {
        /*returns string*/
        debugf_sucess("recieved:");
        debugf_info("%s\n", buffer);
        return buffer;
    }
    else
    {
        return NULL;
    }
}

/**
 * Makes out of an error string a human readable debug statement.
 * Example of an error code would be "#ERRO -23"
 */
void oxy_decode_general_error(const char errorCode_buff[])
{
    s32_t errorCode = 0;
    sscanf(errorCode_buff, "%*c %*c %*c %*c %*c %i", &errorCode);
    switch (errorCode)
    {
    case 0:
        debugf_error("Error in error code function (seufts). Pretty sure no connect\n");
        break;
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
        debugf_error("Unknown error code: %i\n", errorCode);
        break;
    }
}

/**/
bool oxy_read_all(struct oxy_mesure *mesure_buffer)
{
    for (uint8_t i = 0; i < 6; i++)
    {
        mesure_buffer->pyro_oxy[i] = i;
        mesure_buffer->pyro_temp[i] = i * 2;
        mesure_buffer->pyro_pressure[i] = i * 3;
    }
    return 1;
}

const s32_t OXY_OPTICALCHANNEL = 1;
const s32_t OXY_SENSORSENABLED = 47; // This parameter defines the enabled sensor types bit for bit
float oxy_meassure(uint8_t number)
{
    /*chooses right oxygen Sensor*/

    /*sends command to messure*/
    char buffer[COMMAND_LENGTH_MAX];
    snprintf(buffer, COMMAND_LENGTH_MAX, "MEA %i %i\r", OXY_OPTICALCHANNEL, OXY_SENSORSENABLED);
    char *buf_return = oxy_commandhandler(buffer, COMMAND_LENGTH_MAX);

    /*decodes answer*/
    struct OxygenReadout OxyReadout;
    int numScanned = sscanf(buf_return, "%*c %*c %*c %*i %*i %i %i %i %i %i %i %i %i %i %i %i %i",
                            &OxyReadout.error,
                            &OxyReadout.dphi,
                            &OxyReadout.umolar,
                            &OxyReadout.mbar,
                            &OxyReadout.airSat,
                            &OxyReadout.tempSample,
                            &OxyReadout.tempCase,
                            &OxyReadout.signalIntensity,
                            &OxyReadout.ambientLight,
                            &OxyReadout.pressure,
                            &OxyReadout.resistorTemp,
                            &OxyReadout.percentOtwo);

    if (numScanned != 11)
    {
        debugf_warn("oxy_read returned only %i from 11 values\n", numScanned);
    }
    // Ausgabe der Strukturwerte
    debugf_info("Error: %i\n", OxyReadout.error);
    debugf_info("Dphi: %i\n", OxyReadout.dphi);
    debugf_info("Umolar: %i\n", OxyReadout.umolar);
    debugf_info("Mbar: %i\n", OxyReadout.mbar);
    debugf_info("AirSat: %i\n", OxyReadout.airSat);
    debugf_info("TempSample: %i\n", OxyReadout.tempSample);
    debugf_info("TempCase: %i\n", OxyReadout.tempCase);
    debugf_info("SignalIntensity: %i\n", OxyReadout.signalIntensity);
    debugf_info("AmbientLight: %i\n", OxyReadout.ambientLight);
    debugf_info("Pressure: %i\n", OxyReadout.pressure);
    debugf_info("ResistorTemp: %i\n", OxyReadout.resistorTemp);
    debugf_info("PercentOtwo: %i\n", OxyReadout.percentOtwo);

    oxy_decode_mesurement_errors(OxyReadout.error); // if theres an error this will print a debug statement

    free_ifnotnull(buf_return);
    return 1.0;
}

/*decodes the R0 Error bit of the MEA Messurement command and prints debug statement*/
void oxy_decode_mesurement_errors(const u32_t R0)
{
    if (R0 & (1 << 0))
    {
        debugf_error("WARNING - automatic amplification level active\n");
    }
    if (R0 & (1 << 1))
    {
        debugf_error("WARNING - sensor signal intensity low\n");
    }
    if (R0 & (1 << 2))
    {
        debugf_error("ERROR - optical detector saturated\n");
    }
    if (R0 & (1 << 3))
    {
        debugf_error("WARNING - reference signal intensity too low\n");
    }
    if (R0 & (1 << 4))
    {
        debugf_error("ERROR - reference signal too high\n");
    }
    if (R0 & (1 << 5))
    {
        debugf_error("ERROR - failure of sample temperature sensor (e.g. Pt100)\n");
    }
    if (R0 & (1 << 6))
    {
        debugf_error("reserved\n");
    }
    if (R0 & (1 << 7))
    {
        debugf_error("WARNING high humidity (>90%%RH) within the module\n");
    }
    if (R0 & (1 << 8))
    {
        debugf_error("ERROR - failure of case temperature sensor\n");
    }
    if (R0 & (1 << 9))
    {
        debugf_error("ERROR - failure of pressure sensor\n");
    }
    if (R0 & (1 << 10))
    {
        debugf_error("ERROR - failure of humidity sensor\n");
    }
}

/**
 * Sets Temperature, pressure and salinity(salzgehalt) of probe.
 * Required for correct oxygen messurement.
 *@param channel Optical channel number. Set C=1
 *@param temp Temperature of the calibration standard in C
 **/
void oxy_calibrate_probe(const uint8_t channel, const s32_t temp, const s32_t pressure, const s32_t salinity)
{
    // char buffer[COMMAND_LENGTH_MAX];
    // snprintf(buffer, COMMAND_LENGTH_MAX, "WTM %s %s %s", OXY_OPTICALCHANNEL, SETTINGS_REGISTER, START_REGISTER, temp);
    // char *buf_return = oxy_commandhandler(buffer);
    // free_ifnotnull(buf_return);

    // save calibration with SVG
}

/**
 * Writes to registers on the oxygen sensor
 * @param register_block Register block number.
T=0: Settings registers
T=1: Calibration registers
T=3: Results registers
T=4: AnalogOutput registe
 * @param frist_registerStart starting register number (R=0 for starting with the first register)
 * @param nValues Number of registers to write.
 * @param  buffer_values buffer of Register values to be written
 **/
void oxy_write_register(const s32_t register_block, const s32_t first_register, const s32_t nValues, const s32_t buffer_values[])
{
    char buffer[COMMAND_LENGTH_MAX];
    /*write command string*/
    snprintf(buffer, COMMAND_LENGTH_MAX, "WTM %i %i %i ", OXY_OPTICALCHANNEL, register_block, first_register);

    /*add values(parameters) to command string*/
    char temp_buffer[COMMAND_LENGTH_MAX];

    for (size_t i = 0; i < nValues; i++) // Loop through the buffer_values array and convert each number to a string
    {
        sprintf(temp_buffer, "%d", buffer_values[i]); // Use sprintf to format the integer into temp_buffer
        strcat(buffer, temp_buffer);

        if (i < (nValues - 1))
        {
            strcat(buffer, " ");
        }
    }

    /*send ready string to sensor*/
    debugf_status("oxy_write_register:%s", buffer);
    char *buf_return = oxy_commandhandler(buffer);
    free_ifnotnull(buf_return);
}

/**
 * Calibrate oxygen Sensor at ambient air
*@param channel Optical channel number. Set C=1
*@param temp Temperature of the calibration standard in 100 * C
*@param pressure Ambient air pressure in 100 * mbar
*@param humidity Relative humidity of the ambient air in units of 10-3 %RH (e.g. 50000
means 50%RH) Set H=100000 (=100%RH) for calibrations in air saturated
water
 **/
void oxy_calibrateOxy_air(const uint8_t channel, const uint temp, const uint pressure, const uint humidity)
{
    char buffer[COMMAND_LENGTH_MAX];
    snprintf(buffer, COMMAND_LENGTH_MAX, "CHI %u %u %u", temp, pressure, humidity);
    char *buf_return = oxy_commandhandler(buffer);
    free_ifnotnull(buf_return);

    // save calibration with SVG
}

/**
 * Calibrate oxygen Sensor at 0* Oxy
 *@param channel Optical channel number. Set C=1
 *@param temp Temperature of the calibration standard in C
 **/
void oxy_calibrateOxy_zero(const uint8_t channel, const uint temp)
{
    char buffer[COMMAND_LENGTH_MAX];
    snprintf(buffer, COMMAND_LENGTH_MAX, "CLO %u", temp);
    char *buf_return = oxy_commandhandler(buffer);
    free_ifnotnull(buf_return);

    // save calibration with SVG
}

/*save config in flash*/
void oxy_saveconfig(const uint8_t channel)
{
    oxy_commandhandler("SVS");
}

/*prints info about*/
void oxy_info(const int8_t channel)
{
    char *buf_return = oxy_commandhandler("VERS");
    /*recieve info*/
    debugf_info("Device info:\n Device_Id nOpticalChannels FirmwareVersion SensorTypes FirmwareBuildNumber Features\n %s", buf_return);
    free_ifnotnull(buf_return);
}
