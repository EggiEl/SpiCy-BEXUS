#include "header.h"
#include "debug_in_color.h"

#include "Adafruit_LTR390.h"

Adafruit_LTR390 ltr = Adafruit_LTR390();

volatile char light_init = 0;

void light_setup()
{
    debugf_status("setup light Sensors\n");

    // pinMode(PIN_LIGHT_SDA_0, OUTPUT);
    // pinMode(PIN_LIGHT_SCL_0, OUTPUT);
    Wire.setSDA(PIN_LIGHT_SDA_0);
    Wire.setSCL(PIN_LIGHT_SCL_0);
    Wire.setTimeout(TIMEOUT_LIGHT_SENSOR);
    Wire.begin();

    Wire1.setSDA(PIN_LIGHT_SDA_1);
    Wire1.setSCL(PIN_LIGHT_SCL_1);
    Wire1.setTimeout(TIMEOUT_LIGHT_SENSOR);
    Wire1.begin();

    if (!ltr.begin(&Wire))
    {
        if (!ltr.begin(&Wire1))
        {
            debug("Couldn't find LTR sensor!");
            light_init = 0;
            return;
        }
    }

    ltr.setMode(LTR390_MODE_UVS);
    if (ltr.getMode() == LTR390_MODE_ALS)
    {
        debug("In ALS mode");
    }
    else
    {
        debug("In UVS mode");
    }

    ltr.setGain(LIGHT_LTR390_GAIN);
    debug("Gain : ");
    switch (ltr.getGain())
    {
    case LTR390_GAIN_1:
        debug(1);
        break;
    case LTR390_GAIN_3:
        debug(3);
        break;
    case LTR390_GAIN_6:
        debug(6);
        break;
    case LTR390_GAIN_9:
        debug(9);
        break;
    case LTR390_GAIN_18:
        debug(18);
        break;
    }

    ltr.setResolution(LTR390_RESOLUTION_20BIT);
    debug("Resolution : ");
    switch (ltr.getResolution())
    {
    case LTR390_RESOLUTION_13BIT:
        debug(13);
        break;
    case LTR390_RESOLUTION_16BIT:
        debug(16);
        break;
    case LTR390_RESOLUTION_17BIT:
        debug(17);
        break;
    case LTR390_RESOLUTION_18BIT:
        debug(18);
        break;
    case LTR390_RESOLUTION_19BIT:
        debug(19);
        break;
    case LTR390_RESOLUTION_20BIT:
        debug(20);
        break;
    }

    ltr.setThresholds(100, 1000);
    ltr.configInterrupt(true, LTR390_MODE_UVS);
}

/**
 * returns light values. make the buffer 14 flaots long
 * 6x values and 1x temperature with the first sensor connected
 * 6x values and 1x temperature with the second sensor connected
 */
void light_read(float buffer[14])
{
    if (!light_init)
    {
        light_setup();
    }

    debugf_status("Readout of light sensor ");

    if (ltr.newDataAvailable())
    {
        uint32_t readout_uv = ltr.readUVS(); // == 0xFFFFFFFF of failure
        debugf_info("UV data: %.1f", (float)readout_uv);
        buffer[0] = (LIGHT_LTR390_WFAC * (float)readout_uv) / (LIGHT_LTR390_UV_SENSITIFITY);
        buffer[1] = (float)readout_uv;
        buffer[2] = readout_uv;

        uint32_t readout_als = ltr.readALS(); // == 0xFFFFFFFF of failure
        debugf_info("UV data: %.1f", (float)readout_als);
        buffer[3] = (LIGHT_LTR390_WFAC * 0.6 * (float)readout_als) / (LIGHT_LTR390_GAIN * LIGHT_LTR390_INT);
        buffer[4] = (float)readout_als;
        buffer[5] = readout_als;
    }
}