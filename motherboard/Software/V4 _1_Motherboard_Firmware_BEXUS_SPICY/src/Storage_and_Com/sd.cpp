#include "header.h"
#include "debug_in_color.h"
#include <SD.h>
#include <SPI.h>
// if testing via breadboard make sure to just connect the SD Card and disconnecting the LAN

void printDirectory(File dir, int numTabs);
void sd_printinfo();

File root;
volatile char sd_init = 0;

void sd_setup()
{
  debugf_status("<sd_init>\n");
  if (MISO_SPI0 == 0 || MISO_SPI0 == 4 || MISO_SPI0 == 16 || MISO_SPI0 == 20)
  {
    SPI.setRX(MISO_SPI0);
    SPI.setTX(MOSI_SPI0);
    SPI.setSCK(SCK_SPI0);
    sd_init = SD.begin(CS_SD);
  }
  else if (MISO_SPI0 == 8 || MISO_SPI0 == 12)
  {
    SPI1.setRX(MISO_SPI0);
    SPI1.setTX(MOSI_SPI0);
    SPI1.setSCK(SCK_SPI0);
    sd_init = SD.begin(CS_SD, SPI1);
  }
  else
  {
    error_handler(ERROR_SD_PINMAP, ERROR_DESTINATION_NO_SD);
    debugf_error("ERROR: Unknown Sd SPI Configuration\n");
    return;
  }

  if (!sd_init)
  {
    error_handler(ERROR_SD_INI, ERROR_DESTINATION_NO_SD);
    return;
  }
  else
  {
    sd_printinfo();
    debugf_sucess("SD init succesfulll\n");
  }
}

/*returns number of saved packets in a file*/
int sd_numpackets(const char filepath[])
{
  if (!sd_init)
  {
    sd_setup();
  }
  File myFile = SD.open(filepath, FILE_WRITE);
  if (myFile)
  {
    return myFile.position() / sizeof(struct packet);
  }
  else
  {
    error_handler(ERROR_SD_COUNT);
    debugf_error("counting SD packets in \"\\%s\" failed\n", filepath);
    return -1;
  }
}

// writes a packet to sd card
bool sd_writestruct(struct packet *s_out, const char filepath[])
{
  debugf_status("SD_writestru.id: %u file: %s", s_out->id, filepath);
  if (!sd_init)
  {
    sd_setup();
  }

  char buffer[sizeof(struct packet)];
  packettochar(s_out, buffer);

  File myFile = SD.open(filepath, FILE_WRITE);
  if (myFile)
  { // if the file opened okay, write to it:
    myFile.write(buffer, sizeof(struct packet));
    myFile.close();
    return 1;
  }
  else
  {
    error_handler(ERROR_WR_STR);
    debugln("-error:opening-failed-");
    return 0;
  }
}

// reads a packet from sd card @position = position of struct packet
bool sd_readstruct(struct packet *data, const char filepath[], unsigned long position)
{
  debug("-{SD_readstru-file:");
  debug(filepath);
  if (!sd_init)
  {
    sd_setup();
  }
  File myFile = SD.open(filepath, FILE_READ);
  if (myFile)
  {
    if (!myFile.available())
    { // can be openend but is empty
      debug("-SDopeningsuccess-error:size(file)=");
      debug(myFile.available());
      debugln("}-");
      return 0;
    }
    uint8_t *buffer = (uint8_t *)malloc(sizeof(struct packet));

    if (!myFile.seek(position * sizeof(struct packet)))
    {
      debugln("-error:SDpositioning failed}-");
      free_ifnotnull(buffer);
      return 0;
    }
    myFile.read(buffer, sizeof(struct packet));
    memcpy(data, buffer, sizeof(struct packet));
    free_ifnotnull(buffer);
    myFile.close();
    debugln("-sucess}-");
    return 1;
  }
  else
  {
    debugln("-error:SDopening-failed}-");
    return 0;
  }
}

/*Writes in the file test.txt the String "testing 1, 2, 3."*/
bool sd_writetofile(const char *buffer_text, const char *filename)
{
  if (!sd_init)
  {
    sd_setup();
  }
  File myFile = SD.open(filename, FILE_WRITE);
  if (myFile)
  { // if the file opened okay, write to it:
    debugf_status("Writing to %s \n", filename);
    myFile.println(buffer_text);
    myFile.close();
  }
  else
  {
    error_handler(ERROR_SD_WRITE_OPEN, ERROR_DESTINATION_NO_SD);
    debugf_info("error opening %s \n", filename); // if the file didn't open, print an error:
    sd_init = 0;
  }
  return 0;
}

/*prints everything from a file*/
bool sd_printfile(const char filepath[])
{
  if (!sd_init)
  {
    sd_setup();
  }
  File myFile = SD.open(filepath);
  if (myFile)
  {
    debugf_info("test.txt:");
    while (myFile.available())
    { // read from the file until there's nothing else in it:
      Serial.write(myFile.read());
    }
    myFile.close();
  }
  else
  {

    debugf_info("error opening test.txt"); // if the file didn't open, print an error:
  }
  return 1;
}

void printDirectory(File dir, int numTabs)
{
  for (uint16_t i = 0; i < 1000; i++) // max. 1000 files printed
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      break; // no more files
    }

    for (uint8_t a = 0; a < numTabs; a++)
    {
      debugf_info("\t");
    }
    debugf_info("%s", entry.name());
    if (entry.isDirectory())
    {
      debugf_info("/\n");
      printDirectory(entry, numTabs + 1);
    }
    else
    {
      debugf_info("\t%ukb\n", entry.size()); // files have sizes, directories do not
    }
    entry.close();
  }
}

void sd_printinfo()
{
  // print the type of card
  debugf_info("Card type:\n");
  switch (SD.type())
  {
  case 0:
    debugf_info("SD1\n");
    break;
  case 1:
    debugf_info("SD2\n");
    break;
  case 3:
    debugf_info("SDHC/SDXC\n");
    break;
  default:
    debugf_info("Unknown\n");
  }
  // debugf_info("Cluster size: %i\nBlocks x Cluster: %i\nBlocks size:%i\nTotal Blocks:%i\nTotal Cluster:%i\n",
  //             SD.clusterSize(), SD.blocksPerCluster(), SD.blockSize(), SD.totalBlocks(), SD.totalClusters());
  uint32_t volumesize;
  debugf_info("Volume type is: %i FAT\n", SD.fatType());
  volumesize = SD.totalClusters();
  volumesize *= SD.clusterSize();
  volumesize /= 1000;

  debugf_info("Volume size %.2fGb| %.fMb| %.f kb\n", (float)(volumesize / 1024) / 1024.0, (float)volumesize / 1024, (float)volumesize);
  debugf_info("Card size:%u\n", SD.size() / 1000);
  // FSInfo fs_info;
  // SDFS.info(fs_info);
  // debugf_info("Total bytes: %u", fs_info.totalBytes);
  // debugf_info("Used bytes: %u", +fs_info.usedBytes);
  root = SD.open("/");
  printDirectory(root, 0);
}