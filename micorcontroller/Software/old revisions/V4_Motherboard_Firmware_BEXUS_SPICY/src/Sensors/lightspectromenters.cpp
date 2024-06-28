#include "header.h"

#include "AS726X.h" //https://github.com/sparkfun/SparkFun_AS726X_Arduino_Library/tree/master

AS726X light_spectro; // https://cdn.sparkfun.com/assets/c/2/9/0/a/AS7265x_Datasheet.pdf

char light_init = 0;

void light_setup()
{
    debugf_status("setup light Sensors\n");
    pinMode(PIN_LIGHT_SDA, OUTPUT);
    pinMode(PIN_LIGHT_SCL, OUTPUT);
    Wire.setSDA(PIN_LIGHT_SDA);
    Wire.setSCL(PIN_LIGHT_SCL);
    Wire.setTimeout(100);
    Wire.begin();
    // if (light_spectro.begin(Wire,GAIN,MEASUREMENT_MODE) == false)
    if (light_spectro.begin(Wire) == false)
    {
        debugf_error("Sensor not connected.\n");
        light_init = 0;
        return;
    }
    light_init = 1;
}

void read_light(bool with_light)
{
    if (!light_init)
    {
        light_setup();
    }

    debugf_status("Readout of light sensor ");
    if (!light_spectro.isConnected())
    {
        debugf_error("light sensor not connected\n");
        return;
    }

    if (with_light)
    {
        light_spectro.takeMeasurementsWithBulb(); // This is a hard wait while all 18 channels are measured
    }
    else
    {
        light_spectro.takeMeasurements();
    }

    if (light_spectro.getVersion() == SENSORTYPE_AS7262)
    {
        debugf_info("AS7262\n"); // Visible readings
        debugf_info("410nm: %.2f µW/cm²\n", light_spectro.getCalibratedViolet());
        debugf_info("435nm: %.2f µW/cm²\n", light_spectro.getCalibratedBlue());
        debugf_info("460nm: %.2f µW/cm²\n", light_spectro.getCalibratedGreen());
        debugf_info("485nm: %.2f µW/cm²\n", light_spectro.getCalibratedYellow());
        debugf_info("510nm: %.2f µW/cm²\n", light_spectro.getCalibratedOrange());
        debugf_info("560nm: %.2f µW/cm²\n", light_spectro.getCalibratedRed());
    }
    else if (light_spectro.getVersion() == SENSORTYPE_AS7263)
    {
        debugf_info("AS7263\n"); // Near IR readings
        debugf_info("610nm: %.2f µW/cm²\n", light_spectro.getCalibratedR());
        debugf_info("645nm: %.2f µW/cm²\n", light_spectro.getCalibratedS());
        debugf_info("680nm: %.2f µW/cm²\n", light_spectro.getCalibratedT());
        debugf_info("730nm: %.2f µW/cm²\n", light_spectro.getCalibratedU());
        debugf_info("760nm: %.2f µW/cm²\n", light_spectro.getCalibratedV());
        debugf_info("860nm: %.2f µW/cm²\n", light_spectro.getCalibratedW());
    }

    debugf_info("Temperature: %u°C\n", light_spectro.getTemperature());
}