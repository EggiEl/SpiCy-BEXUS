#include "header.h"
#include "debug_in_color.h"
#if USB_ENABLE ==1
#include <LittleFS.h>
#include <SingleFileDrive.h>

uint32_t cnt = 0;
bool okayToWrite = true;
char singleFileUsb_init = 0;

void usb_singlefile_setup()
{
    Serial.begin();
    delay(1000);

    LittleFS.begin();

    // Set up the USB disk share
    singleFileDrive.onDelete(deleteCSV);
    singleFileDrive.onPlug(plug);
    singleFileDrive.onUnplug(unplug);
    singleFileDrive.begin("data.csv", "Recorded data from the Raspberry Pi Pico.csv");

    // Find the last written data
    File f = LittleFS.open("data.csv", "r");
    if (!f || !f.size())
    {
        cnt = 1;
        headerCSV();
    }
    else
    {
        if (f.size() > 2048)
        {
            f.seek(f.size() - 1024);
        }
        do
        {
            String s = f.readStringUntil('\n');
            sscanf(s.c_str(), "%lu,", &cnt);
        } while (f.available());
        f.close();
        cnt++;
    }
    singleFileUsb_init = 1;
    Serial.printf("Starting acquisition at %lu\n", cnt);
}

void usb_singlefile_update()
{
    if (!singleFileUsb_init)
    {
        usb_singlefile_setup();
    }
    float temp = analogReadTemp();
    uint32_t hwrand = rp2040.hwrand32();
    // Make sure the USB connect doesn't happen while we're writing!
    noInterrupts();
    unplug(1);
    if (okayToWrite)
    {
        Serial.printf("Sampling...%lu\n", cnt);
        // Don't want the USB to connect during an update!
        File f = LittleFS.open("data.csv", "a");
        if (f)
        {
            f.printf("%lu,%lu,%f,%lu\n", cnt++, millis(), temp, hwrand);
            f.close();
            debugln("USB single File content updated, eject to display changes.");
        }
    }
    plug(1);
    interrupts();
}

// Make the CSV file and give it a simple header
void headerCSV()
{
    File f = LittleFS.open("data.csv", "w");
    f.printf("sample,millis,temp,rand\n");
    f.close();
    cnt = 0;
}

// Called when the USB stick connected to a PC and the drive opened
// Note this is from a USB IRQ so no printing to SerialUSB/etc.
void plug(uint32_t i)
{
    (void)i;
    okayToWrite = false;
}

// Called when the USB is ejected or removed from a PC
// Note this is from a USB IRQ so no printing to SerialUSB/etc.
void unplug(uint32_t i)
{
    (void)i;
    okayToWrite = true;
}

// Called when the PC tries to delete the single file
// Note this is from a USB IRQ so no printing to SerialUSB/etc.
void deleteCSV(uint32_t i)
{
    (void)i;
    debug("singleusbfile deleted by user\n");
    LittleFS.remove("data.csv");
    headerCSV();
}

void singlefile_close()
{
    LittleFS.remove("data.csv");
    LittleFS.end();
    debugln("USB single File closed. May in need of an reboot (/r)");
}

#endif