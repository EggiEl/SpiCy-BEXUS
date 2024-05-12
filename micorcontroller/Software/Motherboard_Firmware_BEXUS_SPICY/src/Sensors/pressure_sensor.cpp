#include "header.h"
#include "MS5611.h"
#include "Wire.h"

// MS5611 MS5611(0x77);

void pressure_setup()
{
    // Wire.begin();
    // Wire.setSDA(12);
    // Wire.setSCL(13);

    // if (MS5611.begin() == true)
    // {
    //     Serial.println("MS5611 found.");
    // }
    // else
    // {
    //     Serial.println("MS5611 not found. halt.");            ;
    // }
    // Serial.println();
}

void pressure_loop()
{
    // MS5611.read(); //  note no error checking => "optimistic".
    // Serial.print("T:\t");
    // Serial.print(MS5611.getTemperature(), 2);
    // Serial.print("\tP:\t");
    // Serial.print(MS5611.getPressure(), 2);
    // Serial.println();
    // delay(1000);
}