#include "header.h"
// https://eu.mouser.com/ProductDetail/TDK/NTCGS163JF103FT8?qs=dbcCsuKDzFU4pk95bZlq7w%3D%3D&countryCode=DE&currencyCode=EUR
// https://www.mouser.de/ProductDetail/Amphenol-Advanced-Sensors/JI-103C1R2-L301?qs=JUmsgfbaopRXkasA8RUqKg%3D%3D&countryCode=DE&currencyCode=EUR

char temp_init = 0;

void temp_setup()
{
    pinMode(PIN_TEMPADC, INPUT);
    pinMode(PIN_PROBEMUX_0, OUTPUT);
    pinMode(PIN_PROBEMUX_1, OUTPUT);
    pinMode(PIN_PROBEMUX_2, OUTPUT);
    pinMode(PIN_MUX_OXY_DISABLE, OUTPUT);
    digitalWrite(PIN_MUX_OXY_DISABLE, 0);
    temp_init = 1;
}

/**
 * Reads out every Thermistor.
 * @param buffer pointer to an array with 8 floats
 **/
void temp_read_all(float *buffer)
{
    buffer[0] = temp_read_one(NTC_0);
    buffer[1] = temp_read_one(NTC_1);
    buffer[2] = temp_read_one(NTC_2);
    buffer[3] = temp_read_one(NTC_3);
    buffer[4] = temp_read_one(NTC_4);
    buffer[5] = temp_read_one(NTC_5);
    buffer[6] = temp_read_one(NTC_SMD);
    buffer[7] = temp_read_one(NTC_10kfix);
}

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

/**
 * Reads out the temperature of one NTC thermistor.
 * Temperature range with current resistor values: 0.25 til 44.85°C
 * @param Number Output Number of the MUX selecting the NTC
 * @return temperature value or -1000000 if NTC is not connected/vaulty
 */
float temp_read_one(uint8_t Number, uint8_t nTimes)
{
    static const float R43_paral_R41 = R41 * R43 / (R41 + R43);
    static const float gain = 1.0 / (R43_paral_R41 / (R43_paral_R41 + R53));
    static const float R41_paral_R41 = 0.5 * R41;
    static const float voffset = VCC_NTC * R41_paral_R41 / (R41_paral_R41 + R43);

    if (!temp_init)
    {
        temp_setup();
    }

    /*Connect the right one*/
    uint8_t A0 = (Number - 1) & 0b00000001;
    uint8_t A1 = ((Number - 1) & 0b00000010) >> 1;
    uint8_t A2 = ((Number - 1) & 0b00000100) >> 2;

    digitalWrite(PIN_PROBEMUX_0, A0);
    digitalWrite(PIN_PROBEMUX_1, A1);
    digitalWrite(PIN_PROBEMUX_2, A2);
    // debugf_info("Select S%u|A0:%u|A1:%u|A2:%u\n", Number, A0, A1, A2);

    /*Read out ADC*/
    analogReadResolution(ADC_RES);
    float voltage_adc = 0;
    for (int i = 0; i < nTimes; i++)
    {
        voltage_adc += (float)(analogRead(PIN_TEMPADC) / ADC_MAX_READ) * VCC_NTC;
    }
    voltage_adc = voltage_adc / nTimes;

    if (voltage_adc < 0.02)
    {
        return -1000000;
    }
    /*Converts the adc voltage after the opamp circuit to the voltage on the ntc*/
    float volt_ntc = voltage_adc * (1.0 / gain) + voffset;

    /*Converts ADC value to a Resistance*/
    float resistance = (float)R_SERIES * (((float)VCC_NTC / volt_ntc) - 1);

    // Calculate the temperature in Kelvin using the Steinhart–Hart  equation
    float NTC_B = NTC_B_AMPHENOL;
    switch (Number) // selects the right NTC_B value for each NTC type
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
    float tempK = 1.0 / (1.0 / NTC_T0 + (1.0 / NTC_B) * log(resistance / NTC_R0));

    // Convert temperature from Kelvin to Celsius
    float tempC = tempK - 273.15;
    // debugf_info("VADC:%.2f VNTC:%.2f R:%.2f T:%.2f\n", voltage_adc, volt_ntc, resistance, tempC);
    return tempC;
}

void temp_print_ntc(uint8_t Pin)
{
    static int temp_start = 1;
    if (temp_start)
    {
        sd_writetofile("timestamp[ms];temperature[°C]", "log_thermal.csv");
        temp_start = 0;
    }
    float temp = temp_read_one(Pin);
    debugf_info("%f°C\n", temp);
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%u;%f", millis(), temp);
    sd_writetofile(buffer, "log_thermal.csv");
}

void temp_record_temp(uint8_t NTC_Probe, uint8_t NTC_Ambient, unsigned long t_nextmeas_ms)
{
    static uint8_t init = 0;
    if (!init)
    {
        sd_writetofile("timestamp[ms];temp_probe[°C];temp_ambient[°C]", "ESA_2W5_cooled.csv");
        init = 1;
    }

    static unsigned long timestamp = millis() + t_nextmeas_ms;
    if (millis() > timestamp)
    {
        timestamp = millis() + t_nextmeas_ms;
        char string[200];
        snprintf(string, sizeof(string), "%u;%.2f;%.2f;", millis(), temp_read_one(NTC_Probe), temp_read_one(NTC_Ambient));
        sd_writetofile(string, "ESA_2W5_cooled.csv");
    }
}