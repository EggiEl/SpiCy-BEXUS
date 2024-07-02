#include <stdio.h>
void convert_bin_to_csv(const char *bin_filename, const char *csv_filename);

struct packet                         // 2L 2f 6L 6I 6I 6I 8I 6I 18f 20s
{                                     // struct_format L L 6L 6f 6f 6i i f 2i 80s
    unsigned int id = 0;              // each packet has a unique id
    unsigned int timestampPacket = 0; // in ms
    float power[2] = {0};             // battery voltage in mV and current consumption in mA

    unsigned int pyro_timestamp[6] = {0}; //
    unsigned int pyro_temp[6] = {0};      // temp on the sensor pcb in °C * 100
    unsigned int pyro_oxy[6] = {0};       // Oxygenvalue
    unsigned int pyro_pressure[6] = {0};  // pressure?
    float light[12] = {0.0f};

    /**temperature from thermistors:
     *0-5 NTC cable
     *6 Nlog regelnTC SMD
     *7 fix reference value
     *8 cpu temp*/
    unsigned int thermistor[9] = {0};
    unsigned int heaterPWM[6] = {0}; // power going to heating
    float pid[6 * 3] = {0};
};

int main()
{
    printf("Converting data.bin in the folder of this exe to data.csv\n");
    convert_bin_to_csv("data.bin", "data.csv");
    printf("done\n");
    return 0;
}

void convert_bin_to_csv(const char *bin_filename, const char *csv_filename)
{
    FILE *bin_file = fopen(bin_filename, "rb");
    if (!bin_file)
    {
        perror("Failed to open binary file");
        return;
    }

    FILE *csv_file = fopen(csv_filename, "w");
    if (!csv_file)
    {
        perror("Failed to open CSV file");
        fclose(bin_file);
        return;
    }

    // Write CSV header
    fprintf(csv_file, "id,timestampPacket,power[0],power[1],");
    for (int i = 0; i < 6; ++i)
    {
        fprintf(csv_file, "pyro_timestamp[%d],pyro_temp[%d],pyro_oxy[%d],pyro_pressure[%d],", i, i, i, i);
    }
    for (int i = 0; i < 12; ++i)
    {
        fprintf(csv_file, "light[%d],", i);
    }
    for (int i = 0; i < 9; ++i)
    {
        fprintf(csv_file, "thermistor[%d],", i);
    }
    for (int i = 0; i < 6; ++i)
    {
        fprintf(csv_file, "heaterPWM[%d],", i);
    }
    for (int i = 0; i < 18; ++i)
    {
        fprintf(csv_file, "pid[%d]", i);
        if (i < 17)
        {
            fprintf(csv_file, ",");
        }
    }
    fprintf(csv_file, "\n");

    struct packet pkt;
    while (fread(&pkt, sizeof(struct packet), 1, bin_file) == 1)
    {
        fprintf(csv_file, "%u,%u,%.2f,%.2f,", pkt.id, pkt.timestampPacket, pkt.power[0], pkt.power[1]);
        for (int i = 0; i < 6; ++i)
        {
            fprintf(csv_file, "%u,%u,%u,%u,", pkt.pyro_timestamp[i], pkt.pyro_temp[i], pkt.pyro_oxy[i], pkt.pyro_pressure[i]);
        }
        for (int i = 0; i < 12; ++i)
        {
            fprintf(csv_file, "%.2f,", pkt.light[i]);
        }
        for (int i = 0; i < 9; ++i)
        {
            fprintf(csv_file, "%u,", pkt.thermistor[i]);
        }
        for (int i = 0; i < 6; ++i)
        {
            fprintf(csv_file, "%u,", pkt.heaterPWM[i]);
        }
        for (int i = 0; i < 18; ++i)
        {
            fprintf(csv_file, "%.2f", pkt.pid[i]);
            if (i < 17)
            {
                fprintf(csv_file, ",");
            }
        }
        fprintf(csv_file, "\n");
    }

    fclose(bin_file);
    fclose(csv_file);
}
