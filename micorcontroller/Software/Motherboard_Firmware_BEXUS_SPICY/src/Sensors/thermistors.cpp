#include "header.h"

char temp_init = 0;
void temp_setup()
{
    pinMode(PIN_TEMPADC, INPUT);
    pinMode(PIN_TEMPMUX_0, OUTPUT);
    pinMode(PIN_TEMPMUX_1, OUTPUT);
    pinMode(PIN_TEMPMUX_2, OUTPUT);
    temp_init = 1;
}

/**
 * Reads out every Thermistor connected via cable.
 * First 6 are the connected via cable
 * @return pointer to an array with 6 floats
 **/
float *readThermistors()
{
    float *buf = (float *)malloc(6 * sizeof(float));
    if (!buf)
    {
        return NULL;
    }
    return buf;
}


const int R41 = 10000;
const int R43 = 6500;
const int R53 = 10000;

const int R_SERIES = 5100;
const float NTC_norm_temp = 25;
const float NTC_B = 25;

float readThermistor(uint8_t Number)
{
    if (!temp_init)
    {
        temp_setup();
    }

    /*Connect the right one*/
    digitalWrite(PIN_TEMPMUX_0, Number & 0b00000001);
    digitalWrite(PIN_TEMPMUX_0, (Number & 0b00000010)<<1);
    digitalWrite(PIN_TEMPMUX_0,( Number & 0b00000100)<<2);
    /*Read out ADC*/
    unsigned int adc_buf = analogRead(PIN_TEMPADC);

    /*Converts ADC value to a Resistance*/
    
    /*Converts the Resistance to a Temperature*/
    // Steinhart Hart goes brrrrrrrrrr
    return -1;
}

// float ntc_convertToC(uint32_t adcValue)
// {
//     float rntc = (float)_NTC_R_SERIES / (((float)_NTC_ADC_MAX / (float)adcValue) - 1.0f);
//     float temp;
//     temp = rntc / (float)_NTC_R_NOMINAL;
//     temp = logf(temp);
//     temp /= (float)_NTC_BETA;
//     temp += 1.0f / ((float)_NTC_TEMP_NOMINAL + 273.15f);
//     temp = 1.0f / temp;
//     temp -= 273.15f;
//     return temp;
// }