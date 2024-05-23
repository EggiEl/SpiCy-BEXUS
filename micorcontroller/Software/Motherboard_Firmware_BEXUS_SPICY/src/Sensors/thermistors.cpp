#include "header.h"

char temp_init = 0;

void temp_setup()
{
    pinMode(PIN_TEMPADC, INPUT);
    pinMode(PIN_TEMPMUX_0, INPUT_PULLDOWN);
    pinMode(PIN_TEMPMUX_1, INPUT_PULLDOWN);
    pinMode(PIN_TEMPMUX_2, INPUT_PULLDOWN);
    temp_init = 1;
}

/**
 * Reads out every Thermistor.
 * @return pointer to an array with 6 floats
 **/
float *temp_read_cable()
{
    float *buf = (float *)malloc(8 * sizeof(float));
    if (!buf)
    {
        return NULL;
    }
    buf[0] = temp_read_one(NTC_0);
    buf[1] = temp_read_one(NTC_1);
    buf[2] = temp_read_one(NTC_2);
    buf[3] = temp_read_one(NTC_3);
    buf[4] = temp_read_one(NTC_4);
    buf[5] = temp_read_one(NTC_5);
    buf[6] = temp_read_one(NTC_SMD);
    buf[7] = temp_read_one(NTC_10kfix);

    return buf;
}

/*Circuit design*/
const float VCC_NTC = 3.0; // reference voltage for the NTC Readout
const float R_SERIES = 5200.0; // Fixed resistor value in ohms (10kΩ)
const float R41 = 10000.0;
const float R43 = 6800.0;
const float R53 = 10000.0;
/*Thermistor Stats*/
const float NTC_B = 3425.0; // Beta parameter
const float NTC_T0 = 298.15;   // Reference temperature in Kelvin (25°C)
const float NTC_R0 = 10000.0;  // Resistance at reference temperature (10kΩ)

float temp_read_one(uint8_t Number)
{
    if (!temp_init)
    {
        temp_setup();
    }
    analogReadResolution(ADC_RES);
    /*Connect the right one*/
    uint8_t A0 = (Number - 1) & 0b00000001;
    uint8_t A1 = ((Number - 1) & 0b00000010) >> 1;
    uint8_t A2 = ((Number - 1) & 0b00000100) >> 2;

    digitalWrite(PIN_TEMPMUX_0, A0);
    digitalWrite(PIN_TEMPMUX_1, A1);
    digitalWrite(PIN_TEMPMUX_2, A2);
   
    // debugf_info("Select S%u|A0:%u|A1:%u|A2:%u\n", Number, A0, A1, A2);

    /*Read out ADC*/
    delay(1);
    float voltage_adc = (float)(analogRead(PIN_TEMPADC) / ADC_MAX) * VCC_NTC;

    /*Converts Readout to Voltage on Resistor*/
    float volt_ntc = VCC_NTC * R41 / (R41 + R43) + voltage_adc * R41 / (R41 + R53);

    /*Converts ADC value to a Resistance*/
    float resistance = (float)R_SERIES * (((float)VCC_NTC / volt_ntc) - 1);

    // Calculate the temperature in Kelvin using the Steinhart–Hart  equation
    // 1/T = 1/T0 + 1/B * ln(R/R0)
    float tempK = 1.0 / (1.0 / NTC_T0 + (1.0 / NTC_B) * log(resistance / NTC_R0));

    // Convert temperature from Kelvin to Celsius
    float tempC = tempK - 273.15;
    debugf_info("VADC:%.2f VNTC:%.2f R:%.2f T:%.2f\n", voltage_adc, volt_ntc, resistance, tempC);
    return tempC;
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