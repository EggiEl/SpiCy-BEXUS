#include "header.h"
#include <SD.h>
#include <SPI.h>
// if testing via breadboard make sure to just connect the SD Card and disconnecting the LAN

static unsigned long int max_freq_sd();
void printDirectory(File dir, int numTabs);

File root;
char sd_init = 0;

unsigned long int max_freq_sd()
{
  debugf_status("sd_freq_calib");
  unsigned long int freq = 1000000; // first frequency to check

  if (SD.begin(CS_SD))
  {           // test if SD card is working at all
    SD.end(); // Serial.println("SD card working.");
  }
  else
  {
    debugf_error("SD card failed_default_f\n");
    return 0;
  }

  // tries different speeds

  for (uint8_t overflow = 0; overflow < 50; overflow++)
  { // tries 1M steps
    if (SD.begin(CS_SD, freq))
    {
      SD.end();
      freq = freq + 1000000;
    }
    else
    {
      freq = freq - 1000000;
      break;
    }
  }

  for (uint8_t overflow = 0; overflow < 50; overflow++)
  { // tries 100k steps
    if (SD.begin(CS_SD, freq))
    {
      SD.end();
      freq = freq + 100000;
    }
    else
    {
      freq = freq - 100000;
      break;
    }
  }

  for (uint8_t overflow = 0; overflow < 50; overflow++)
  { // tries 10k steps
    if (SD.begin(CS_SD, freq))
    {
      SD.end();
      freq = freq + 10000;
    }
    else
    {
      freq = freq - 10000;
      break;
    }
  }

  SD.end();
  debugf_sucess("max freq: %i\n", freq);
  return freq;
}

void sd_setup()
{
  bool sdInitialized = false;
  // Ensure the SPI pinout the SD card is connected to is configured properly
  // Select the correct SPI based on MISO_SD pin for the RP2040
  if (MISO_SD == 0 || MISO_SD == 4 || MISO_SD == 16)
  {
    SPI.setRX(MISO_SD);
    SPI.setTX(MOSI_SD);
    SPI.setSCK(SCK_SD);
    sdInitialized = SD.begin(CS_SD);
  }
  else if (MISO_SD == 8 || MISO_SD == 12)
  {
    SPI1.setRX(MISO_SD);
    SPI1.setTX(MOSI_SD);
    SPI1.setSCK(SCK_SD);
    sdInitialized = SD.begin(CS_SD, SPI1);
  }
  else
  {
    Serial.println(F("ERROR: Unknown SPI Configuration"));
    return;
  }

  if (!sdInitialized)
  {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  }
  else
  {
    Serial.println("Wiring is correct and a card is present.");
  }
  
  // 0 - SD V1, 1 - SD V2, or 3 - SDHC/SDXC
  // print the type of card
  Serial.println();
  Serial.print("Card type:         ");
  switch (SD.type())
  {
  case 0:
    Serial.println("SD1");
    break;
  case 1:
    Serial.println("SD2");
    break;
  case 3:
    Serial.println("SDHC/SDXC");
    break;
  default:
    Serial.println("Unknown");
  }

  Serial.print("Cluster size:          ");
  Serial.println(SD.clusterSize());
  Serial.print("Blocks x Cluster:  ");
  Serial.println(SD.blocksPerCluster());
  Serial.print("Blocks size:  ");
  Serial.println(SD.blockSize());

  Serial.print("Total Blocks:      ");
  Serial.println(SD.totalBlocks());
  Serial.println();

  Serial.print("Total Cluster:      ");
  Serial.println(SD.totalClusters());
  Serial.println();

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("Volume type is:    FAT");
  Serial.println(SD.fatType(), DEC);

  volumesize = SD.totalClusters();
  volumesize *= SD.clusterSize();
  volumesize /= 1000;
  Serial.print("Volume size (Kb):  ");
  Serial.println(volumesize);
  Serial.print("Volume size (Mb):  ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Gb):  ");
  Serial.println((float)volumesize / 1024.0);

  Serial.print("Card size:  ");
  Serial.println((float)SD.size() / 1000);

  FSInfo fs_info;
  SDFS.info(fs_info);

  Serial.print("Total bytes: ");
  Serial.println(fs_info.totalBytes);

  Serial.print("Used bytes: ");
  Serial.println(fs_info.usedBytes);

  root = SD.open("/");
  printDirectory(root, 0);
}

/**
 *  initializes the SD card with the corresponding SPI pins
 * @param sd_setup changes this global variable to true if succesfull
 */
void sd_setupa()
{
  debugf_status("<sd_init>\n");

  // pinMode(MISO_SD, OUTPUT);
  pinMode(CS_SD, OUTPUT);
  // pinMode(SCK_SD, OUTPUT);
  // pinMode(MOSI_SD, OUTPUT);

  SPI.setRX(MISO_SD);
  SPI.setTX(MOSI_SD);
  SPI.setSCK(SCK_SD);

  SPI.begin();

  if (!SD.begin(CS_SD))
  {
    debugf_error("sd_init failed\n");
    sd_init = 0;
    // SD.end(); ??
    return;
  }

  unsigned long int maxfreq = max_freq_sd();
  if (maxfreq)
  {
    SD.begin(CS_SD, maxfreq * 0.8);
  }
  else
  {
    debugf_error("Sd freq calibration failed, so SD clock speed is set to 100k. \n");
    SD.begin(CS_SD, 1000000);
  }

  sd_init = 1;
  debugf_sucess("sd_init sucecss\n");
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
    debugf_error("counting SD packets in \"\\%s\" failed\n", filepath);
    return -1;
  }
}

// writes a packet to sd card
bool sd_writestruct(struct packet *s_out, const char filepath[])
{
  debug("-{SD_writestru.id:");
  debug(s_out->id);
  debug("-file:");
  debug(filepath);
  if (!sd_init)
  {
    sd_setup();
  }
  char *buffer = packettochar(s_out);
  File myFile = SD.open(filepath, FILE_WRITE);
  if (myFile)
  { // if the file opened okay, write to it:
    myFile.write(buffer, sizeof(struct packet));
    free(buffer);
    myFile.close();
    debugln("-sucess}-");
    return 1;
  }
  else
  {
    debugln("-error:opening-failed}-");
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
      return 0;
    }
    myFile.read(buffer, sizeof(struct packet));
    memcpy(data, buffer, sizeof(struct packet));
    free(buffer);
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
bool test_writetofile()
{
  if (!sd_init)
  {
    sd_setup();
  }
  File myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile)
  { // if the file opened okay, write to it:
    debug("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    myFile.close();
    Serial.println("done.");
  }
  else
  {
    Serial.println("error opening test.txt"); // if the file didn't open, print an error:
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
    Serial.println("test.txt:");
    while (myFile.available())
    { // read from the file until there's nothing else in it:
      Serial.write(myFile.read());
    }
    myFile.close();
  }
  else
  {
    Serial.println("error opening test.txt"); // if the file didn't open, print an error:
  }
  return 1;
}

void printDirectory(File dir, int numTabs)
{
  while (true)
  {

    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++)
    {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory())
    {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    }
    else
    {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.print(entry.size(), DEC);
      time_t cr = entry.getCreationTime();
      time_t lw = entry.getLastWrite();
      struct tm *tmstruct = localtime(&cr);
      Serial.printf("\tCREATION: %d-%02d-%02d %02d:%02d:%02d", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
      tmstruct = localtime(&lw);
      Serial.printf("\tLAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    }
    entry.close();
  }
}

/*Tests the SD. Work in Progress*/
void testSD()
{
  while (!Serial)
  {
  }

  sd_setup();
  char filepath[] = "test.txt";
  SD.remove(filepath);

  struct packet *data = packet_create();
  sd_writestruct(data, filepath);
  data->info[0] = 100;
  debugln(data->timestampPacket);

  struct packet *data1 = packet_create();
  sd_writestruct(data1, filepath);
  data1->info[0] = 101;
  debugln(data1->timestampPacket);

  debugln(sd_numpackets(filepath));

  struct packet i;
  sd_readstruct(&i, filepath, 0);
  Serial.println(i.info[0], DEC);
  Serial.println(i.timestampPacket);

  sd_readstruct(&i, filepath, 1);
  Serial.println(i.info[0], DEC);
  Serial.println(i.timestampPacket);
}
