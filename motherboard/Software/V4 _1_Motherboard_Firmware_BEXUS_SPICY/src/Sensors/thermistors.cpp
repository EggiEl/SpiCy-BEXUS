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
void temp_read_all(float buffer[8])
{
    buffer[0] = temp_read_one(NTC_PROBE_0);
    buffer[1] = temp_read_one(NTC_PROBE_1);
    buffer[2] = temp_read_one(NTC_PROBE_2);
    buffer[3] = temp_read_one(NTC_PROBE_3);
    buffer[4] = temp_read_one(NTC_PROBE_4);
    buffer[5] = temp_read_one(NTC_PROBE_5);
    buffer[6] = temp_read_one(NTC_SMD);
    buffer[7] = temp_read_one(NTC_PROBE_10kfix);
}

/**
 * @param NTC specify here wich NTC y wanna check. dont specify anything or set to 255 checks the currently connected one
 * @return 1 if connected, 0 if not
 *  */
uint8_t temp_isconnected(uint8_t NTC)
{
    if (temp_read_one(NTC, 5) == -1000000.0)
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
    select_probe_or_NTC(NTC);

    // uint8_t A0 = (NTC - 1) & 0b00000001;
    // uint8_t A1 = ((NTC - 1) & 0b00000010) >> 1;
    // uint8_t A2 = ((NTC - 1) & 0b00000100) >> 2;
    // digitalWrite(PIN_PROBEMUX_0, A0);
    // digitalWrite(PIN_PROBEMUX_1, A1);
    // digitalWrite(PIN_PROBEMUX_2, A2);

    /*Read out ADC*/
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
    }
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
    case NTC_PROBE_10kfix:
        NTC_B = NTC_B_FIX;
        break;
    default:
        NTC_B = NTC_B_AMPHENOL;
        break;
    }

    // Calculate the temperature in Kelvin using the Steinhart–Hart  equation
    float tempK = 1.0 / (1.0 / NTC_T0 + (1.0 / NTC_B) * log(resistance / NTC_R0));

    // Convert temperature from Kelvin to Celsius
    float tempC = tempK - 273.15;
    // debugf_info("VADC:%.2f VNTC:%.2f R:%.2f T:%.2f\n", voltage_adc, volt_ntc, resistance, tempC);
    return tempC;
}

/**
 * logs ntc thermistor reatings over time tot he sd card
 */
void temp_log(const char path[], uint8_t NTC_Probe, uint8_t NTC_Ambient, unsigned long t_nextmeas_ms)
{
    static uint8_t init = 0;
    if (!init)
    {
        sd_writetofile("timestamp[ms];temp_probe[°C];temp_ambient[°C];Voltage[V];Current[A]\n", path);
        init = 1;
    }

    static unsigned long timestamp = millis() + t_nextmeas_ms;
    if (millis() > timestamp)
    {
        timestamp = millis() + t_nextmeas_ms;
        char string[200];
        snprintf(string, sizeof(string), "%u;%.2f;%.2f;%.5f;%.7f", millis(), temp_read_one(NTC_Probe), temp_read_one(NTC_Ambient), get_batvoltage(), get_current());
        sd_writetofile(string, path);
    }
}