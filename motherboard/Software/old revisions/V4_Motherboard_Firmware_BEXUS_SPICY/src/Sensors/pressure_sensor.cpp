#include "header.h"
#include "MS5611.h"
#include "Wire.h"

// MS5611 pressure_sensor;
// // MS5611 MS5611(0x77);
static int PRESSURE_ADRESS = 119;
char pressure_init = 0;

void pressure_setup()
{
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
    Wire.setSDA(12);
    Wire.setSCL(13);
    Wire.setTimeout(1000);
    Wire.begin();
    pressure_init = 1;
}

float pressure_read()
{
    if (!pressure_init)
    {
        pressure_setup();
    }
    // pinMode(12,INPUT_PULLUP);
    // pinMode(13, INPUT_PULLUP);

    // Reset
    Wire.beginTransmission(PRESSURE_ADRESS);
    Wire.write(0x1E);
    Wire.endTransmission();
    delay(10);

    // Read the calibration constants
    uint16_t C[7]={0};
    for (uint8_t i = 1; i < 7; i++)
    {
        uint8_t command = 0b10100000 + (i * 2); // Command to read the i-th calibration constant
        // Start a transmission to the sensor's address
        Wire.beginTransmission(PRESSURE_ADRESS);
        Wire.write(command); // Send the command to read the next calibration constant
        Wire.endTransmission();

        // Request two bytes from the sensor
        Wire.requestFrom(PRESSURE_ADRESS, 2);
        // Wait for the data to be available
        if (Wire.available() < 2)
        {
            Serial.print("Error: Less than 2 bytes available for command ");
            Serial.println(command, HEX);
            Wire.end();
            return -1; // Error case
        }

        // Read the calibration constant
        C[i] = (Wire.read() << 8) | Wire.read();
        debugf_info("C%i| value:%i| com: ", i, C[i]);
        debugln(command, BIN);
    }

    uint8_t *buff = (uint8_t *)calloc(4,1);

    // D1 conversion pressure
    Wire.beginTransmission(PRESSURE_ADRESS);
    Wire.write(0x40);
    Wire.write(0x00);
    Wire.requestFrom(PRESSURE_ADRESS, 2);
    if (Wire.available() < 2)
    {
        Serial.print("Error: Less than 2 bytes available for command ");
        Serial.println(0x48, HEX);
        Wire.end();
        return -1; // Error case
    }
    Wire.endTransmission();
    Wire.readBytes(buff, 4);
    u32_t pressure_raw = (buff[3] << 24) | (buff[2] << 16) | (buff[1] << 8) | buff[0];
    debugf_info("Raw Pressure:%i\n", pressure_raw);

    // D2 conversion temperature
    Wire.beginTransmission(PRESSURE_ADRESS);
    Wire.write(0x58);
    Wire.write(0x00);
    Wire.requestFrom(PRESSURE_ADRESS, 2);
    if (Wire.available() < 2)
    {
        Serial.print("Error: Less than 2 bytes available for command ");
        Serial.println(0x58, HEX);
        Wire.end();
        return -1; // Error case
    }
    Wire.endTransmission();
    Wire.readBytes(buff, 4);
    u32_t temp_raw = (buff[3] << 24) | (buff[2] << 16) | (buff[1] << 8) | buff[0];
    debugf_info("Raw Voltage:%i\n", temp_raw);

    // read ADC
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.end();

    // MS5611.read(); //  note no error checking => "optimistic".
    // Serial.print("T:\t");
    // Serial.print(MS5611.getTemperature(), 2);
    // Serial.print("\tP:\t");
    // Serial.print(MS5611.getPressure(), 2);
    // Serial.println();
    // delay(1000);
    free(buff);
    return -1;
}


MS5611 pressure(PRESSURE_ADRESS);

uint32_t start, stop;

void pressure_example()
{
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
  
    Serial.println();
    Serial.println(__FILE__);
    Serial.print("MS5611_LIB_VERSION: ");
    Serial.println(MS5611_LIB_VERSION);
    Serial.println();
 
    Wire.setSDA(12);
    Wire.setSCL(13);
    Wire.begin();
    if (pressure.begin() == true)
    {
        Serial.println("MS5611 found.");
    }
    else
    {
        Serial.println("MS5611 not found. halt.");
        while (1)
            ;
    }
    Serial.println();

    Serial.println("OSR \t TIME");

    start = micros();
    pressure.read(); // uses default OSR_ULTRA_LOW  (fastest)
    stop = micros();
    Serial.print((uint8_t)pressure.getOversampling());
    Serial.print("\t");
    Serial.println(stop - start);
    delay(10); // to flush serial.

    pressure.setOversampling(OSR_LOW);
    start = micros();
    pressure.read();
    stop = micros();
    Serial.print((uint8_t)pressure.getOversampling());
    Serial.print("\t");
    Serial.println(stop - start);
    delay(10); // to flush serial.

    pressure.setOversampling(OSR_STANDARD);
    start = micros();
    pressure.read();
    stop = micros();
    Serial.print((uint8_t)pressure.getOversampling());
    Serial.print("\t");
    Serial.println(stop - start);
    delay(10); // to flush serial.

    pressure.setOversampling(OSR_HIGH);
    start = micros();
    pressure.read();
    stop = micros();
    Serial.print((uint8_t)pressure.getOversampling());
    Serial.print("\t");
    Serial.println(stop - start);
    delay(10); // to flush serial.

    pressure.setOversampling(OSR_ULTRA_HIGH);
    start = micros();
    pressure.read();
    stop = micros();
    Serial.print((uint8_t)pressure.getOversampling());
    Serial.print("\t");
    Serial.println(stop - start);
    delay(10); // to flush serial.

    Serial.println("\ndone...");
}