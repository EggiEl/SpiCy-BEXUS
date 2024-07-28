#include "header.h"
#include "debug_in_color.h"

volatile char oxy_serial_init = 0;
volatile char oxy_calib = 0;

SerialPIO oxySerial(PIN_OX_TX, PIN_OX_RX);

void oxy_send_dummy();
uint8_t oxy_meassure(const uint8_t Probe_Number, struct OxygenReadout *readout);
void oxy_decode_mesurement_errors(const u32_t R0);
void oxy_decode_general_error(const char errorCode_buff[]);
void oxy_calibrateOxy_air(const uint8_t Probe_Number, const uint temp, const uint pressure, const uint humidity);

/*setups the SoftwareSerial Connection to the oxygen sensor*/
void oxy_serial_setup()
{
    debugf_status("oxy_serial_setup ");
    if (millis() < (3 * 60 * 1000))
    {
        debugf_warn("- warnuptime of 3min not reached -");
    }

    // Serial1.setRX(PIN_OX_RX);
    // Serial1.setTX(PIN_OX_TX);
    // Serial1.setTimeout(OXY_SERIAL_TIMEOUT);
    // Serial1.begin(OXY_BAUD);

    oxySerial.setTimeout(OXY_SERIAL_TIMEOUT); // longst command yet: #LOGO->550ms
    oxySerial.begin(OXY_BAUD);

    // select_probe_or_NTC(NTC_PROBE_0);
    // oxy_send_dummy();

    debugf_sucess("oxy setup was succesfull\n");
    oxy_serial_init = 1;
}

/*sending dummy byte. For syncronising the data line.*/
void oxy_send_dummy()
{
    char buffer[20];
    oxySerial.write("\r");
    oxySerial.flush();
    // returns a error after 10ms
    oxySerial.readBytesUntil('\r', buffer, sizeof(buffer));
    delay(2); // this is necessary
}

/**
 * tests weather a oxygen sensor is connected
 * @param PROBE specify here wich Probe y wanna check. dont specify anything or set to 255 checks the currently connected one
 */
uint8_t oxy_isconnected(const int PROBE)
{
    if (!oxy_serial_init)
    {
        oxy_serial_setup();
    }

    select_probe_or_NTC(PROBE);

    oxy_send_dummy();

    /*sendst test*/
    char buffer[10] = {0};
    oxySerial.write("\r");
    oxySerial.flush();
    oxySerial.readBytes(buffer, 1);
    delay(2); // this is necessary

    if (buffer[0] == '\r')
    {
        return 1;
    }
    else
    {
        // debugf_warn("Return for testing command #LOGO:\"%s\"\n", buffer);
        // oxy_decode_general_error(buffer);
        return 0;
    }
}

/*Starts Console for talk with fd-odem module*/
void oxy_console()
{
    debugf_status("Console for Pyrosience FD-OEM Module\n");
    debugf_info("use /m[probenumber] to trigger a oxygen measurement. probenumber must follow /m with no empty space\n");
    debugf_info("use /c to calibrate.\n");
    debugf_info("use /q to exit.\n");

    if (!oxy_serial_init)
    {
        oxy_serial_setup();
    }

    unsigned char isrunning = 1;
    while (isrunning)
    {
        select_probe_or_NTC(NTC_PROBE_0);

        /*recives & prints data from FD-ODEM*/
        if (oxySerial.available())
        {
            Serial.write(oxySerial.read());
        }

        /*sends data to FD-ODEM*/
        if (Serial.available() >= 3)
        {

            char buffer[COMMAND_LENGTH_MAX + 1] = {0};
            Serial.readBytesUntil('\n', buffer, COMMAND_LENGTH_MAX);
            buffer[COMMAND_LENGTH_MAX] = '\0';

            if (buffer[0] == '/') // filters for commands
            {
                switch (buffer[1])
                {
                case 'q':
                {
                    isrunning = 0;
                    break;
                }
                case 'm':
                {
                    unsigned long timestamp = millis();
                    OxygenReadout readout;
                    oxy_meassure((uint)buffer[2], &readout);
                    debugf_magenta("%u\n", millis() - timestamp);
                    break;
                }
                case 'c':
                {
                    unsigned long timestamp = millis();
                    oxy_calibrateOxy_air((uint)buffer[2], (uint32_t)(temp_read_one((uint)buffer[2]) * 100), 965935UL, 46942UL);
                    debugf_magenta("%u\n", millis() - timestamp);
                    break;
                }
                default:
                    break;
                }
            }
            else // if no command it goes directly to sensor
            {
                oxy_commandhandler(buffer, RETURN_LENGTH_MAX);
            }
        }

        /*resets Watchdog*/
        rp2040.wdt_reset();
    }
    debugf_status("closed FD-OEM console\n");
}

/**
 * Sends a command via SoftwareSerial to a Oxigen Sensor
 * please check befor calling if oxy_serial_init is set
 * @param Command Command and all parameters as a string. dosent matter is y add "/r", works with and without
 * @param returnValues 1: returns a pointer to the readout, 0: returns a NULL as pointer
 * @return success: returns full reply string, error: returns NULL and gives Error the ErrorHandling() function
 */
char *oxy_commandhandler(const char command[], uint8_t returnValues)
{
    if (!oxy_serial_init)
    {
        oxy_serial_setup();
    }

    /*creates return buffer*/
    uint8_t nReturn = RETURN_LENGTH_MAX;

    char *buffer = (char *)calloc(nReturn, sizeof(char));
    if (!buffer)
    {
        debugf_error("oxy commandhandler memory allocation fail\n");
        return NULL;
    }

    /*Sending Command*/
    oxy_send_dummy();

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
        if (!returnValues)
        {
            free_ifnotnull(buffer);
        }
        return buffer;
    }
    else
    {
        free_ifnotnull(buffer);
        return NULL;
    }
}

/**
 * Makes out of an error string a human readable debug statement.
 * Example of an error code would be "#ERRO -23"
 */
void oxy_decode_general_error(const char errorCode_buff[])
{
    int32_t errorCode = 0;
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

/**
 * Reads measurements of all 6 Sensors and puts them in the provided array
 * This should take ~0.943s
 */
uint8_t oxy_read_all(struct OxygenReadout measure_buffer[6])
{
    if (!oxy_serial_init)
    {
        oxy_serial_setup();
    }
    debugf_status("oxy_readall ");
    uint success = 0;
    debugf_status(". ");
    success += oxy_meassure(NTC_PROBE_0, &measure_buffer[0]);
    debugf_status(". ");
    success += oxy_meassure(NTC_PROBE_1, &measure_buffer[1]);
    debugf_status(". ");
    success += oxy_meassure(NTC_PROBE_2, &measure_buffer[2]);
    debugf_status(". ");
    success += oxy_meassure(NTC_PROBE_3, &measure_buffer[3]);
    debugf_status(". ");
    success += oxy_meassure(NTC_4, &measure_buffer[4]);
    debugf_status(". ");
    success += oxy_meassure(NTC_5, &measure_buffer[5]);
    debugf_status("done\n");
    return success;
}

const int32_t OXY_OPTICALCHANNEL = 1;
const int32_t OXY_SENSORSENABLED = 47; // This parameter defines the enabled sensor types bit for bit
/**
 * read all avaliable parameter from one oxygen sensor.
 * this should take ~240ms
 */
uint8_t oxy_meassure(const uint8_t Probe_Number, struct OxygenReadout *readout)
{
    /*chooses right oxygen Sensor*/
    select_probe_or_NTC(Probe_Number);

    readout->timestamp_mesurement = millis();

    oxy_send_dummy();

    /*sends command to messure*/
    char command_buffer[COMMAND_LENGTH_MAX];
    snprintf(command_buffer, COMMAND_LENGTH_MAX, "MEA %i %i\r", OXY_OPTICALCHANNEL, OXY_SENSORSENABLED);
    char *buf_return = oxy_commandhandler(command_buffer, 1);

    /*decodes answer*/
    if (!buf_return)
    { // if no data avaliable, for example when no sensor connected
        // Set all values of the struct to 0 using memset
        memset(readout, 0, sizeof(struct OxygenReadout));
        return 0;
    }

    int numScanned = sscanf(buf_return, "%*c %*c %*c %*d %*d %d %d %d %d %d %d %d %d %d %d %d %d",
                            &readout->error,
                            &readout->dphi,
                            &readout->umolar,
                            &readout->mbar,
                            &readout->airSat,
                            &readout->tempSample,
                            &readout->tempCase,
                            &readout->signalIntensity,
                            &readout->ambientLight,
                            &readout->pressure,
                            &readout->humidity,
                            &readout->resistorTemp,
                            &readout->percentOtwo);

    if (numScanned != 11)
    {
        // debugf_warn("oxy_read returned only %i from 11 values\n", numScanned);
    }

    // Ausgabe der Strukturwerte
    debugf_info("Error: %d\n", readout->error);
    debugf_info("Dphi: %.4f mrad\n", readout->dphi / 1000.0);
    debugf_info("Umolar: %.4f umol/l\n", readout->umolar / 1000.0);
    debugf_info("Mbar: %.4f mbar\n", readout->mbar / 1000.0);
    debugf_info("AirSat: %.4f %%\n", readout->airSat / 1000.0);
    debugf_info("TempSample: %.4f °C\n", readout->tempSample / 1000.0);
    debugf_info("TempCase: %.4f °C\n", readout->tempCase / 1000.0);
    debugf_info("SignalIntensity: %.4fmV\n", readout->signalIntensity / 1000.0);
    debugf_info("AmbientLight: %.4f mV\n", readout->ambientLight / 1000.0);
    debugf_info("Pressure: %.4f mbar\n", readout->pressure / 1000.0);
    debugf_info("Humidity: %.4f %%\n", readout->humidity / 1000.0);
    debugf_info("ResistorTemp: %.4f OHM\n", readout->resistorTemp / 1000.0);
    debugf_info("PercentOtwo: %.4f %%\n", readout->percentOtwo / 1000.0);

    oxy_decode_mesurement_errors(readout->error); // if theres an error this will print a debug statement

    free_ifnotnull(buf_return);
    return 1;
}

/*decodes the R0 Error bit of the MEA Messurement command and prints debug statement*/
void oxy_decode_mesurement_errors(const u32_t R0)
{

    if (R0 & (1 << 0))
    {
        error_handler(ERROR_OXY_AUTO_AMP);
        debugf_error("WARNING - automatic amplification level active\n");
    }
    if (R0 & (1 << 1))
    {
        error_handler(ERROR_OXY_SIGNAL_INT_LOW);
        debugf_error("WARNING - sensor signal intensity low\n");
    }
    if (R0 & (1 << 2))
    {
        error_handler(ERROR_OXY_OPTICAL_DETECTOR_SATURATED);
        debugf_error("ERROR - optical detector saturated\n");
    }
    if (R0 & (1 << 3))
    {
        error_handler(ERROR_OXY_REF_SIGNAL_LOW);
        debugf_error("WARNING - reference signal intensity too low\n");
    }
    if (R0 & (1 << 4))
    {
        error_handler(ERROR_OXY_REF_SIGNAL_HIGH);
        debugf_error("ERROR - reference signal too high\n");
    }
    if (R0 & (1 << 5))
    {
        error_handler(ERROR_OXY_SAMPLE_TEMP_SENSOR);
        debugf_error("ERROR - failure of sample temperature sensor (e.g. Pt100)\n");
    }
    if (R0 & (1 << 6))
    {
        error_handler(ERROR_OXY_RESERVED);
        debugf_error("reserved\n");
    }
    if (R0 & (1 << 7))
    {
        error_handler(ERROR_OXY_HIGH_HUMIDITY);
        debugf_error("WARNING - high humidity (>90\%\%RH) within the module\n");
    }
    if (R0 & (1 << 8))
    {
        error_handler(ERROR_OXY_CASE_TEMP_SENSOR);
        debugf_error("ERROR - failure of case temperature sensor\n");
    }
    if (R0 & (1 << 9))
    {
        error_handler(ERROR_OXY_PRESSURE_SENSOR);
        debugf_error("ERROR - failure of pressure sensor\n");
    }
    if (R0 & (1 << 10))
    {
        error_handler(ERROR_OXY_HUMIDITY_SENSOR);
        debugf_error("ERROR - failure of humidity sensor\n");
    }
}

/**
 * Calibrate oxygen Sensor at ambient air
 * takes around 4483ms
 * @param temp Temperature of the calibration standard in 100 * C
 * @param pressure Ambient air pressure in 100 * mbar
 * @param humidity Relative humidity of the ambient air in units of 10-3 %RH (e.g. 50000
means 50%RH) Set H=100000 (=100%RH) for calibrations in air saturated
water
 **/
void oxy_calibrateOxy_air(const uint8_t Probe_Number, const uint temp, const uint pressure, const uint humidity)
{
    const uint8_t channel = 1;
    char buffer[COMMAND_LENGTH_MAX];

    debugf_status("Calibrating OxySensor %u\n", Probe_Number);
    debugf_info("calibration values: temp= %.4f°C,pressure= %.4fmbar,humidity= %.6f%%\n", (float)(temp / 100.0), (float)(pressure / 100.0), (float)(humidity / 100.0));
    select_probe_or_NTC(Probe_Number);
    oxySerial.setTimeout(7000);

    snprintf(buffer, COMMAND_LENGTH_MAX, "CHI %u %u %u %u", channel, temp, pressure, humidity);
    oxy_commandhandler(buffer);

    oxySerial.setTimeout(OXY_SERIAL_TIMEOUT);
    // save calibration with SVG
    // oxy_commandhandler("SVS_1");
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
    oxy_commandhandler(buffer);

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
void oxy_write_register(const int32_t register_block, const int32_t first_register, const int32_t nValues, const int32_t buffer_values[])
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
    oxy_commandhandler(buffer);
}