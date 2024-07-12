#include "header.h"
#include "debug_in_color.h"
#include <Wire.h>
static void scan_wire_single(unsigned int pinsda, unsigned int pinscl, char Wire_select, unsigned long freq);

/*scan the Wire interfaces for devices. Only works on RP2040 boards.*/
void scan_wire()
{
    SET_COLOUR_YELLOW
    debugln("\n<I2C Scan>");
    SET_COLOUR_RESET
    unsigned long freq = 100000;
    scan_wire_single(0, 1, 0, freq);
    scan_wire_single(2, 3, 1, freq);
    scan_wire_single(4, 5, 0, freq);
    scan_wire_single(6, 7, 1, freq);
    scan_wire_single(8, 9, 0, freq);
    scan_wire_single(10, 11, 1, freq);
    scan_wire_single(12, 13, 0, freq);
    scan_wire_single(14, 15, 1, freq);
    scan_wire_single(16, 17, 0, freq);
    scan_wire_single(18, 19, 1, freq);
    scan_wire_single(20, 21, 0, freq);
    scan_wire_single(22, 23, 1, freq);
    scan_wire_single(24, 25, 0, freq);
    scan_wire_single(26, 27, 1, freq);
    scan_wire_single(28, 29, 0, freq);

    debugln("I2C Scan complete.\n");
}

/*needed for scan wire*/
void scan_wire_single(unsigned int pinsda, unsigned int pinscl, char Wire_select, unsigned long freq)
{
    pinMode(pinsda, OUTPUT);
    pinMode(pinscl, OUTPUT);
    TwoWire *wireToUse;
    if (!Wire_select)
    {
        wireToUse = &Wire;
    }
    else
    {
        wireToUse = &Wire1;
    }

    wireToUse->setSDA(pinsda);
    wireToUse->setSCL(pinscl);
    wireToUse->setClock(freq);
    wireToUse->begin();

    unsigned int nDevices = 0;
    for (byte address = 1; address < 127; address++)
    {
        // The i2c_scanner uses the return value of the Write.endTransmisstion to see if a device did acknowledge to the address.
        wireToUse->beginTransmission(address);
        byte error = wireToUse->endTransmission();

        if (error == 0)
        {
            debugf_info("adr: %i| sda: %i| scl: %i| wire: %i|freq: %i\n", address, pinsda, pinscl, Wire_select,freq);
            nDevices++;
        }
        else if (error == 4)
        {
            debug("Unknown error at address 0x");
            if (address < 16)
            {
                debug("0");
            }
            debugln(address);
        }
    }
    if (nDevices == 0)
    {
        // debugln("No I2C devices found\n");
    }
    else
    {
        // debugln("Scan Complete\n");
    }
    wireToUse->end();
}
