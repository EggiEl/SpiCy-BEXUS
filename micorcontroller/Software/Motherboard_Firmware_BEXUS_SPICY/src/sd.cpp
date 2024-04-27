#include "header.h"
#include <SD.h>
// if testing via breadboard make sure to just connect the SD Card and disconnecting the LAN
static unsigned long int max_freq_sd();

char SD_init = 0;

unsigned long int max_freq_sd()
{
  debugf_yellow("sd_freq_calib");
  unsigned long int freq = 1000000; // first frequency to check
  SPI.setRX(MISO_SD);
  SPI.setTX(MOSI_SD);
  SPI.setSCK(SCK_SD);

  if (SD.begin(CS_SD))
  {           // test if SD card is working at all
    SD.end(); // Serial.println("SD card working.");
  }
  else
  {
    debugf_red("SD card failed_default_f\n");
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
  debugf_green("max freq: %i\n", freq);
  return freq;
}

// initializes the SD card with the corresponding SPI pins
void init_SD()
{
  debugf_yellow("<SD_init>\n");
  SPI.setRX(MISO_SD);
  SPI.setTX(MOSI_SD);
  SPI.setSCK(SCK_SD);
  if (!SD.begin(CS_SD, 1000000))
  {
    debugf_red("SD_init failed\n");
    SD_init = 0;
  }
  else
  {
    SD.end();
    unsigned int maxfreq = max_freq_sd();
    if (maxfreq)
    {
      SD.begin(CS_SD, maxfreq * 0.8);
    }
    else
    {
      debugf_red("Sd freq calibration failed, so SD clock speed is set to 100k. \n");
      SD.begin(CS_SD, 1000000);
    }
    SD_init = 1;
    debugf_green("SD_init sucecss\n");
  }
}

/*returns number of saved packets in a file*/
int sd_numpackets(const char filepath[])
{
  if (!SD_init)
  {
    init_SD();
  }
  File myFile = SD.open(filepath, FILE_WRITE);
  if (myFile)
  {
    return myFile.position() / sizeof(struct packet);
  }
  else
  {
    debugf_red("counting SD packets in \"\\%s\" failed\n",filepath);
    return -1;
  }
}

// writes a packet to sd card
bool writestruct(struct packet s_out, const char filepath[])
{
  debug("-{SD_writestru.id:");
  debug(s_out.id);
  debug("-file:");
  debug(filepath);
  if (!SD_init)
  {
    init_SD();
  }
  uint8_t *buffer = (uint8_t *)malloc(sizeof(struct packet));
  memcpy(buffer, &s_out, sizeof(struct packet)); // converts struct in a char array
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
bool readstruct(struct packet *data, const char filepath[], unsigned long position)
{
  debug("-{SD_readstru-file:");
  debug(filepath);
  if (!SD_init)
  {
    init_SD();
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
  if (!SD_init)
  {
    init_SD();
  }
  File myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile)
  { // if the file opened okay, write to it:
    Serial.print("Writing to test.txt...");
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
bool printfile(const char filepath[])
{
  if (!SD_init)
  {
    init_SD();
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

/*Tests the SD. Work in Progress*/
void testSD()
{
  while (!Serial)
  {
  }

  init_SD();
  char filepath[] = "test.txt";
  SD.remove(filepath);

  struct packet data = packet_create();
  writestruct(data, filepath);
  data.info[0] = 100;
  debugln(data.timestampPacket);

  struct packet data1 = packet_create();
  writestruct(data1, filepath);
  data1.info[0] = 101;
  debugln(data1.timestampPacket);

  debugln(sd_numpackets(filepath));

  struct packet i;
  readstruct(&i, filepath, 0);
  Serial.println(i.info[0], DEC);
  Serial.println(i.timestampPacket);

  readstruct(&i, filepath, 1);
  Serial.println(i.info[0], DEC);
  Serial.println(i.timestampPacket);
}
