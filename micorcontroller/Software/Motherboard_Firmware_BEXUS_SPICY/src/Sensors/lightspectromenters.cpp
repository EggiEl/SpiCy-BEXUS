#include "header.h"

#include "SparkFun_AS7265X.h" //Click here to get the library: http://librarymanager/All#SparkFun_AS7265X
AS7265X light_spectro;

char light_init = 0;

void light_setup()
{
    Serial.begin(115200);
    Serial.println("AS7265x Spectral Triad Example");

    if (light_spectro.begin() == false)
    {
        Serial.println("Sensor does not appear to be connected. Please check wiring. Freezing...");
        light_init = 0;
        return;
    }
    light_init = 1;
}

void read_light()
{
    if (!light_init)
    {
        light_setup();
        if (!light_init)
        {
            debugf_error("setup failed\n");
            return;
        }
    }

    light_spectro.takeMeasurements(); // This is a hard wait while all 18 channels are measured

    Serial.print(light_spectro.getCalibratedA()); // 410nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedB()); // 435nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedC()); // 460nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedD()); // 485nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedE()); // 510nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedF()); // 535nm
    Serial.print(",");

    Serial.print(light_spectro.getCalibratedG()); // 560nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedH()); // 585nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedR()); // 610nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedI()); // 645nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedS()); // 680nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedJ()); // 705nm
    Serial.print(",");

    Serial.print(light_spectro.getCalibratedT()); // 730nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedU()); // 760nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedV()); // 810nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedW()); // 860nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedK()); // 900nm
    Serial.print(",");
    Serial.print(light_spectro.getCalibratedL()); // 940nm
    Serial.print(",");

    Serial.println();
}