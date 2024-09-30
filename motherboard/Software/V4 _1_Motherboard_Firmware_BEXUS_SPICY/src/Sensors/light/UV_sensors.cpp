#include "header.h"
#include "debug_in_color.h"

#include "Adafruit_LTR390.h"

Adafruit_LTR390 ltr = Adafruit_LTR390();

volatile char light_init = 0;

void light_setup()
{
    debugf_status("setup light Sensors\n");

    Wire.setSDA(PIN_LIGHT_SDA_1);
    Wire.setSCL(PIN_LIGHT_SCL_1);
    Wire.setTimeout(TIMEOUT_LIGHT_SENSOR);

    Wire1.setSDA(PIN_LIGHT_SDA_0);
    Wire1.setSCL(PIN_LIGHT_SCL_0);
    Wire1.setTimeout(TIMEOUT_LIGHT_SENSOR);

    if (!ltr.begin(&Wire))
    {
        if (!ltr.begin(&Wire1))
        {
            debugf_warn("Couldn't find LTR sensor!\n");
            light_init = 0;
            return;
        }
    }

    ltr.setMode(LTR390_MODE_UVS);
    if (ltr.getMode() == LTR390_MODE_ALS)
    {
        debugf_info("ALS mode|");
    }
    else
    {
        debugf_info("UVS mode|");
    }

    ltr.setGain(LIGHT_LTR390_GAIN);
    debugf_info("Gain : ");
    switch (ltr.getGain())
    {
    case LTR390_GAIN_1:
        debugf_info("1");
        break;
    case LTR390_GAIN_3:
        debugf_info("3");
        break;
    case LTR390_GAIN_6:
        debugf_info("6");
        break;
    case LTR390_GAIN_9:
        debugf_info("9");
        break;
    case LTR390_GAIN_18:
        debugf_info("18");
        break;
    }
    debugf_info("|");

    ltr.setResolution(LTR390_RESOLUTION_20BIT);
    debugf_info("Resolution : ");
    switch (ltr.getResolution())
    {
    case LTR390_RESOLUTION_13BIT:
        debugf_info("13");
        break;
    case LTR390_RESOLUTION_16BIT:
        debugf_info("16");
        break;
    case LTR390_RESOLUTION_17BIT:
        debugf_info("17");
        break;
    case LTR390_RESOLUTION_18BIT:
        debugf_info("18");
        break;
    case LTR390_RESOLUTION_19BIT:
        debugf_info("19");
        break;
    case LTR390_RESOLUTION_20BIT:
        debugf_info("20");
        break;
    }
    debugf_info("\n");

    // ltr.setThresholds(100, 1000);
    ltr.configInterrupt(false, LTR390_MODE_UVS);
    light_init = 1;
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
        return;
    }

    // debugf_status("Readout of light sensor\n");
    if (ltr.newDataAvailable())
    {
        uint32_t readout_uv = ltr.readUVS(); // == 0xFFFFFFFF of failure
        float uv_in_lux = (LIGHT_LTR390_WFAC * (float)readout_uv) / (LIGHT_LTR390_UV_SENSITIFITY);
        // debugf_info("UV data %f lux, %u raw\n", uv_in_lux, readout_uv);
        buffer[0] = uv_in_lux;
        buffer[1] = (float)readout_uv;
        buffer[2] = readout_uv;

        /*
                ltr.setMode(LTR390_MODE_UVS);
                ltr.setResolution(LTR390_RESOLUTION_16BIT);
                delay(25);

                uint32_t readout_als = ltr.readALS(); // == 0xFFFFFFFF of failure
                float afls_in_lix = (LIGHT_LTR390_WFAC * 0.6 * (float)readout_als) / (LIGHT_LTR390_GAIN * LIGHT_LTR390_INT);
                debugf_info("ASL data: %f flux, %u raw\n", afls_in_lix, readout_als);
                buffer[3] = afls_in_lix;
                buffer[4] = (float)readout_als;
                buffer[5] = readout_als;

                ltr.setMode(LTR390_MODE_UVS);
                 ltr.setResolution(LTR390_RESOLUTION_20BIT);
        */
    }
}


