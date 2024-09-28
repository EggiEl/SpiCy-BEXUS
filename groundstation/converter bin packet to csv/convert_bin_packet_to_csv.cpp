#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void convert_bin_to_csv(const char *bin_filename, const char *csv_filename);
struct OxygenReadout
{
    int32_t error = 0;
    int32_t dphi = 0;
    int32_t umolar = 0;
    int32_t mbar = 0;
    int32_t airSat = 0;
    int32_t tempSample = 0;
    int32_t tempCase = 0;
    int32_t signalIntensity = 0;
    int32_t ambientLight = 0;
    int32_t pressure = 0;
    int32_t humidity = 0;
    int32_t resistorTemp = 0;
    int32_t percentOtwo = 0;
    unsigned long timestamp_mesurement = 0;
};
struct packet
{                                     // struct_format L L 6L 6f 6f 6i i f 2i 80s
    unsigned int id = 0;              // each packet has a unique id
    unsigned int timestampPacket = 0; // in ms
    float power[2] = {0};             // battery voltage in mV and current consumption in mA

    struct OxygenReadout oxy_measure[6];
    float light[12] = {0.0f};

    /**temperature from thermistors:
     *0-5 NTC cable
     *6 NTC SMD
     *7 fix reference value
     *8 cpu temp*/
    float thermistor[9] = {0};
    float heaterPWM[6] = {0}; // power going to heating
    float pid[3] = {0};
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *file_path = argv[1];

    // Attempt to open the file to ensure it exists and is accessible
    FILE *file = fopen(file_path, "rb");
    if (file == NULL)
    {
        perror("fopen");
        printf("add file name as parameter when calling the exe.\n");
        return EXIT_FAILURE;
    }
    fclose(file);

    /*generates out file from input file*/
    char file_path_csv[300];
    snprintf(file_path_csv, 300, "%s.csv", file_path);

    /*start conversion*/
    printf("Converting -%s- in the folder of this exe to -%s-\n", file_path, file_path_csv);
    convert_bin_to_csv(file_path, file_path_csv);
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

    /*Write CSV header*/
    fprintf(csv_file, "id,timestampPacket,Vbat,Ibat,");

    for (int i = 0; i < 6; ++i)
    {
        fprintf(csv_file, "oxy_error[%d],", i);
        fprintf(csv_file, "dpho[%d],", i);
        fprintf(csv_file, "umolar[%d],", i);
        fprintf(csv_file, "mbar[%d],", i);
        fprintf(csv_file, "airSat[%d],", i);
        fprintf(csv_file, "tempSample[%d],", i);
        fprintf(csv_file, "tempCase[%d],", i);
        fprintf(csv_file, "signalIntens[%d],", i);
        fprintf(csv_file, "ambiLight[%d],", i);
        fprintf(csv_file, "pressure[%d],", i);
        fprintf(csv_file, "humidity[%d],", i);
        fprintf(csv_file, "reistorTEmp[%d],", i);
        fprintf(csv_file, "percentOxy[%d],", i);
        fprintf(csv_file, "timestamp[%d],", i);
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

    for (int i = 0; i < 3; ++i)
    {
        fprintf(csv_file, "pid[%d],", i);
    }

    fprintf(csv_file, "\n");

    // printf("header done\n");

    /*writing data*/
    unsigned int counter = 0;
    struct packet pkt;
    while (fread(&pkt, sizeof(struct packet), 1, bin_file) == 1)
    {
        // static int a = 0;
        // if (a != 0)
        // {
        //     printf("\r");
        // }
        counter++;
        // printf("writing line %d", a);
        // a++;

        fprintf(csv_file, "%u,%u,%f,%f,", pkt.id, pkt.timestampPacket, pkt.power[0], pkt.power[1]);

        for (int i = 0; i < 6; ++i)
        {
            fprintf(csv_file, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%u,",
                    pkt.oxy_measure[i].error,
                    pkt.oxy_measure[i].dphi,
                    pkt.oxy_measure[i].umolar,
                    pkt.oxy_measure[i].mbar,
                    pkt.oxy_measure[i].airSat,
                    pkt.oxy_measure[i].tempSample,
                    pkt.oxy_measure[i].tempCase,
                    pkt.oxy_measure[i].signalIntensity,
                    pkt.oxy_measure[i].ambientLight,
                    pkt.oxy_measure[i].pressure,
                    pkt.oxy_measure[i].humidity,
                    pkt.oxy_measure[i].resistorTemp,
                    pkt.oxy_measure[i].percentOtwo,
                    pkt.oxy_measure[i].timestamp_mesurement);
        }

        for (int i = 0; i < 12; ++i)
        {
            fprintf(csv_file, "%f,", pkt.light[i]);
        }
        for (int i = 0; i < 9; ++i)
        {
            fprintf(csv_file, "%f,", pkt.thermistor[i]);
        }
        for (int i = 0; i < 6; ++i)
        {
            fprintf(csv_file, "%f,", pkt.heaterPWM[i]);
        }

        for (int i = 0; i < 3; ++i)
        {
            fprintf(csv_file, "%f,", pkt.pid[i]);
        }

        fprintf(csv_file, "\n");
    }

    printf("wrote lines: %u\n", counter);
    fclose(bin_file);
    fclose(csv_file);
}
