#include "header.h"
#include "debug_in_color.h"
// https://eu.mouser.com/ProductDetail/TDK/NTCGS163JF103FT8?qs=dbcCsuKDzFU4pk95bZlq7w%3D%3D&countryCode=DE&currencyCode=EUR
// https://www.mouser.de/ProductDetail/Amphenol-Advanced-Sensors/JI-103C1R2-L301?qs=JUmsgfbaopRXkasA8RUqKg%3D%3D&countryCode=DE&currencyCode=EUR

volatile char temp_init = 0;

void temp_setup()
{
    pinMode(PIN_TEMPADC, INPUT);
    analogReadResolution(ADC_RES);
    temp_init = 1;
}

/**
 * Reads out every Thermistor.
 * 0-5 NTC cable
 * 6 NTC SMD
 * 7 fix reference value
 **/
void temp_read_all(float buffer[AMOUNT_NTC_THERMISTORS])
{
    buffer[0] = temp_read_one(NTC_OR_OxY_0);
    buffer[1] = temp_read_one(NTC_OR_OxY_1);
    buffer[2] = temp_read_one(NTC_OR_OxY_2);
    buffer[3] = temp_read_one(NTC_OR_OxY_3);
    buffer[4] = temp_read_one(NTC_4);
    buffer[5] = temp_read_one(NTC_5);
    buffer[6] = temp_read_one(NTC_SMD);
    buffer[7] = temp_read_one(NTC_10kfix);
}

/**
 * @param NTC specify here wich NTC y wanna check. dont specify anything or set to 255 checks the currently connected one
 * @return 1 if connected, 0 if not
 *  */
uint8_t temp_isconnected(uint8_t NTC)
{
    if (temp_read_one(NTC, 1) == -1000000.0)
    {
        return 0;
    }
    return 1;
}

/**
 * Reads out the temperature of one NTC thermistor.
 * Temperature range with current resistor values: 0.25 til 44.85°C
 * @param NTC Output NTC of the MUX selecting the NTC
 * @param nTimes dictates over how many measurement the values should be meaned. Set to 1 to just read out once.
 * @return temperature value or -1000000 if NTC is not connected/vaulty
 */
float temp_read_one(uint8_t NTC, uint8_t nTimes)
{
    /*circuit design*/
    const float VCC_NTC = 3.0;     // reference voltage for the NTC Readout
    const float R_SERIES = 5200.0; // Fixed resistor value in ohms (10kΩ)
    const float R41 = 10000.0;
    const float R43 = 51000.0;
    const float R53 = 10000.0;

    /*thermistor stats*/
    const float NTC_B_AMPHENOL = 3977.0; // Beta parameter
    const float NTC_B_SMD = 3435.0;      // Beta parameter
    const float NTC_B_FIX = 3977.0;
    const float NTC_T0 = 298.15;  // Reference temperature in Kelvin (25°C)
    const float NTC_R0 = 10000.0; // Resistance at reference temperature (10kΩ)

    /*calculations*/
    const float R43_paral_R41 = R41 * R43 / (R41 + R43);
    const float gain = 1.0 / (R43_paral_R41 / (R43_paral_R41 + R53));
    const float R41_paral_R41 = 0.5 * R41;
    const float voffset = VCC_NTC * R41_paral_R41 / (R41_paral_R41 + R43);

    if (!temp_init)
    {
        temp_setup();
    }

    /*Connect the right one*/
    select_oxy_or_ntc(NTC);

    /* check if NTC connected*/
    if (analogRead(PIN_TEMPADC) > ADC_MAX_READ * 0.05)
    {
        // error_handler(ERROR_NO_NTC_CONNECTED);
        return -1000000;
    }

    /*old simple way
    Read out ADC
    float voltage_adc = 0;
    for (int i = 0; i < nTimes; i++)
    {
        voltage_adc += (float)(analogRead(PIN_TEMPADC) / ADC_MAX_READ) * VCC_NTC;
    }
    voltage_adc = voltage_adc / nTimes;
    if (voltage_adc < 0.02)
    {
        // error_handler(ERROR_NO_NTC_CONNECTED);
        return -1000000;
    }*/

    /*Read out ADC in a buffer and calculating adc_average*/
    float adc_average = 0;
    float adc_buffer[nTimes];
    for (int i = 0; i < nTimes; i++)
    {
        float adc_redout = (float)analogRead(PIN_TEMPADC);
        adc_buffer[i] = adc_redout;
        adc_average += adc_redout;
    }
    adc_average = adc_average / nTimes;

    /*removing spikes in adc_buffer*/
    const float tolerance = 0.2;
    float voltage_adc;
    for (int i = 0; i < nTimes; i++)
    {
        if (!((1 - tolerance) * adc_average < adc_buffer[i] < adc_average * (1 + tolerance)))
        // replaces adc_buffer values which dividate from the adc_average about more than tolerance
        {
            adc_buffer[i] = adc_average;
        }
    }

    /*calculating new adc_average without spikes*/
    float filtered_adc_average = 0;
    for (int i = 0; i < nTimes; i++)
    {
        filtered_adc_average += adc_buffer[i];
    }
    filtered_adc_average = filtered_adc_average / nTimes;

    /*calculating volgae form adc value*/
    voltage_adc = (filtered_adc_average / ADC_MAX_READ) * VCC_NTC;

    /*Converts the adc voltage after the opamp circuit to the voltage on the ntc*/
    float volt_ntc = voltage_adc * (1.0 / gain) + voffset;

    /*Converts ADC value to a Resistance*/
    float resistance = (float)R_SERIES * (((float)VCC_NTC / volt_ntc) - 1);

    // selects the right NTC_B value for each NTC type
    float NTC_B = NTC_B_AMPHENOL;
    switch (NTC)
    {
    case NTC_SMD:
        NTC_B = NTC_B_SMD;
        break;
    case NTC_10kfix:
        NTC_B = NTC_B_FIX;
        break;
    default:
        NTC_B = NTC_B_AMPHENOL;
        break;
    }

    // Calculate the temperature in Kelvin using the Steinhart–Hart equation
    float tempK = 1.0 / (1.0 / NTC_T0 + (1.0 / NTC_B) * log(resistance / NTC_R0));

    // Convert temperature from Kelvin to Celsius
    float tempC = tempK - 273.15;
    // debugf_info("VADC:%.2f VNTC:%.2f R:%.2f T:%.2f\n", voltage_adc, volt_ntc, resistance, tempC);
    return tempC;
}

