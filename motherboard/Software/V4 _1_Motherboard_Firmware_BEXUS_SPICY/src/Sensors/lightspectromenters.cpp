#include "header.h"
#include "debug_in_color.h"

#include "AS726X.h" //https://github.com/sparkfun/SparkFun_AS726X_Arduino_Library/tree/master

AS726X light_spectro_AS7262; // https://cdn.sparkfun.com/assets/c/2/9/0/a/AS7265x_Datasheet.pdf
AS726X light_spectro_AS7263; // https://cdn.sparkfun.com/assets/c/2/9/0/a/AS7265x_Datasheet.pdf

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
    // if (light_spectro_AS7262.begin(Wire,GAIN,MEASUREMENT_MODE) == false)
    if (!light_spectro_AS7262.begin(Wire))
    {
        debugf_error("Sensor not connected.\n");
        light_init = 0;
    }

    Wire1.setSDA(PIN_LIGHT_SDA_1);
    Wire1.setSCL(PIN_LIGHT_SCL_1);
    Wire1.setTimeout(TIMEOUT_LIGHT_SENSOR);
    Wire1.begin();
    // if (light_spectro_AS7262.begin(Wire,GAIN,MEASUREMENT_MODE) == false)
    if (!light_spectro_AS7262.begin(Wire))
    {
        debugf_error("Sensor not connected.\n");
        light_init = 0;
    }
    light_init = 1;
}

/**
 * returns light values. make the buffer 14 flaots long
 * 6x values and 1x temperature, 2 times for 2 Sensors
 */
void light_read(float *buffer, bool with_flash)
{
    if (!light_init)
    {
        light_setup();
    }

    debugf_status("Readout of light sensor ");
    if (!light_spectro_AS7262.isConnected())
    {
        debugf_error("light sensor not connected\n");
        return;
    }

    if (with_flash)
    {
        light_spectro_AS7262.takeMeasurementsWithBulb(); // This is a hard wait while all 18 channels are measured
        light_spectro_AS7263.takeMeasurementsWithBulb();
    }
    else
    {
        light_spectro_AS7262.takeMeasurements();
        light_spectro_AS7263.takeMeasurements();
    }

    if (light_spectro_AS7262.getVersion() == SENSORTYPE_AS7262)
    {
        debugf_info("AS7262\n"); // Visible readings
        debugf_info("410nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedViolet());
        debugf_info("435nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedBlue());
        debugf_info("460nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedGreen());
        debugf_info("485nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedYellow());
        debugf_info("510nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedOrange());
        debugf_info("560nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedRed());
        debugf_info("Temperature: %u°C\n", light_spectro_AS7262.getTemperature());
        buffer[0] = light_spectro_AS7262.getCalibratedViolet();
        buffer[1] = light_spectro_AS7262.getCalibratedBlue();
        buffer[2] = light_spectro_AS7262.getCalibratedGreen();
        buffer[3] = light_spectro_AS7262.getCalibratedYellow();
        buffer[4] = light_spectro_AS7262.getCalibratedOrange();
        buffer[5] = light_spectro_AS7262.getCalibratedRed();
        buffer[6] = light_spectro_AS7262.getTemperature();
    }
    else if (light_spectro_AS7262.getVersion() == SENSORTYPE_AS7263)
    {
        debugf_info("AS7263\n"); // Near IR readings
        debugf_info("610nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedR());
        debugf_info("645nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedS());
        debugf_info("680nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedT());
        debugf_info("730nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedU());
        debugf_info("760nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedV());
        debugf_info("860nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedW());
        debugf_info("Temperature: %u°C\n", light_spectro_AS7262.getTemperature());
        buffer[0] = light_spectro_AS7262.getCalibratedR();
        buffer[1] = light_spectro_AS7262.getCalibratedS();
        buffer[2] = light_spectro_AS7262.getCalibratedT();
        buffer[3] = light_spectro_AS7262.getCalibratedU();
        buffer[4] = light_spectro_AS7262.getCalibratedV();
        buffer[5] = light_spectro_AS7262.getCalibratedW();
        buffer[6] = light_spectro_AS7262.getTemperature();
    }

    if (light_spectro_AS7262.getVersion() == SENSORTYPE_AS7262)
    {
        debugf_info("AS7262\n"); // Visible readings
        debugf_info("410nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedViolet());
        debugf_info("435nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedBlue());
        debugf_info("460nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedGreen());
        debugf_info("485nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedYellow());
        debugf_info("510nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedOrange());
        debugf_info("560nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedRed());
        debugf_info("Temperature: %u°C\n", light_spectro_AS7262.getTemperature());
        buffer[7] = light_spectro_AS7262.getCalibratedViolet();
        buffer[8] = light_spectro_AS7262.getCalibratedBlue();
        buffer[9] = light_spectro_AS7262.getCalibratedGreen();
        buffer[10] = light_spectro_AS7262.getCalibratedYellow();
        buffer[11] = light_spectro_AS7262.getCalibratedOrange();
        buffer[12] = light_spectro_AS7262.getCalibratedRed();
        buffer[13] = light_spectro_AS7262.getTemperature();
    }
    else if (light_spectro_AS7262.getVersion() == SENSORTYPE_AS7263)
    {
        debugf_info("AS7263\n"); // Near IR readings
        debugf_info("610nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedR());
        debugf_info("645nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedS());
        debugf_info("680nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedT());
        debugf_info("730nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedU());
        debugf_info("760nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedV());
        debugf_info("860nm: %.2f µW/cm²\n", light_spectro_AS7262.getCalibratedW());
        debugf_info("Temperature: %u°C\n", light_spectro_AS7262.getTemperature());
        buffer[7] = light_spectro_AS7262.getCalibratedR();
        buffer[8] = light_spectro_AS7262.getCalibratedS();
        buffer[9] = light_spectro_AS7262.getCalibratedT();
        buffer[10] = light_spectro_AS7262.getCalibratedU();
        buffer[11] = light_spectro_AS7262.getCalibratedV();
        buffer[12] = light_spectro_AS7262.getCalibratedW();
        buffer[13] = light_spectro_AS7262.getTemperature();
    }
}