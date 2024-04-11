#define MISO_SD 0
#define MOSI_SD 3
#define CS_SD 1
#define SCK_SD 2

//if testing via breadboard make sure to just connect the SD Card and not the LAN
volatile uint8_t SD_init = 0;

unsigned long int max_freq_sd() {
  debug("-{sd_freq_calib-");
  unsigned long int freq = 1000000;  //first frequency to check
  SPI.setRX(MISO_SD);
  SPI.setTX(MOSI_SD);
  SPI.setSCK(SCK_SD);

  if (SD.begin(CS_SD)) {  // test if SD card is working at all
    SD.end();             // Serial.println("SD card working.");
  } else {
    debug("failed_default_f}-");
    return 0;
  }

  //tries different speeds

  for (uint8_t overflow = 0; overflow < 50; overflow++) {  //tries 1M steps
    if (SD.begin(CS_SD, freq)) {
      SD.end();
      freq = freq + 1000000;
    } else {
      freq = freq - 1000000;
      break;
    }
  }

  for (uint8_t overflow = 0; overflow < 50; overflow++) {  //tries 100k steps
    if (SD.begin(CS_SD, freq)) {
      SD.end();
      freq = freq + 100000;
    } else {
      freq = freq - 100000;
      break;
    }
  }

  for (uint8_t overflow = 0; overflow < 50; overflow++) {  //tries 10k steps
    if (SD.begin(CS_SD, freq)) {
      SD.end();
      freq = freq + 10000;
    } else {
      freq = freq - 10000;
      break;
    }
  }

  SD.end();
  debug("-freq:");
  debug(freq);
  debug("-sucess}-");
  return freq;
}

// initializes the SD card with the corresponding SPI pins
void init_SD() {
  if (SD_init) {debug("-SD_init=true-");}
  debug("-{SD_init-");
  SPI.setRX(MISO_SD);
  SPI.setTX(MOSI_SD);
  SPI.setSCK(SCK_SD);
  if (!SD.begin(CS_SD, max_freq_sd() * 0.8)) {
    debugln("failed}-");
    SD_init = 0;
  } else {
    debugln("sucess}-");
    SD_init = 1;
  }
}

/*returns number of saved packets in a file*/
int sd_numpackets(const char filepath[]) {
  debug("-{SD_sd_sizesavedpackets-file:");
  debug(filepath);
  if (!SD_init) { init_SD(); }
  File myFile = SD.open(filepath, FILE_WRITE);
  if (myFile) {
    debug("numberpackets:");
    debug(myFile.position() / sizeof(struct packet));
    debugln("-sucess}-");
    return myFile.position() / sizeof(struct packet);
  } else {
    debugln("-error:SDopening-failed}-");
    return -1;
  }
}

// writes a packet to sd card
bool writestruct(struct packet s_out, const char filepath[]) {
  debug("-{SD_writestru.id:");
  debug(s_out.id);
  debug("-file:");
  debug(filepath);
  if (!SD_init) { init_SD(); }
  uint8_t* buffer = (uint8_t*)malloc(sizeof(struct packet));
  memcpy(buffer, &s_out, sizeof(struct packet));  //converts struct in a char array
  File myFile = SD.open(filepath, FILE_WRITE);
  if (myFile) {  // if the file opened okay, write to it:
    myFile.write(buffer, sizeof(struct packet));
    free(buffer);
    myFile.close();
    debugln("-sucess}-");
    return 1;
  } else {
    debugln("-error:opening-failed}-");
    return 0;
  }
}

// reads a packet from sd card @position = position of struct packet
bool readstruct(struct packet* data, const char filepath[], unsigned long position) {
  debug("-{SD_readstru-file:");
  debug(filepath);
  if (!SD_init) { init_SD(); }
  File myFile = SD.open(filepath, FILE_READ);
  if (myFile) {
    if (!myFile.available()) {  //can be openend but is empty
      debug("-SDopeningsuccess-error:size(file)=");
      debug(myFile.available());
      debugln("}-");
      return 0;
    }
    uint8_t* buffer = (uint8_t*)malloc(sizeof(struct packet));
    if (!myFile.seek(position * sizeof(struct packet))) {
      debugln("-error:SDpositioning failed}-");
      return 0;
    }
    myFile.read(buffer, sizeof(struct packet));
    memcpy(data, buffer, sizeof(struct packet));
    free(buffer);
    myFile.close();
    debugln("-sucess}-");
    return 1;
  } else {
    debugln("-error:SDopening-failed}-");
    return 0;
  }
}

/*Writes in the file test.txt the String "testing 1, 2, 3."*/
bool test_writetofile() {
  if (!SD_init) { init_SD(); }
  File myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {  // if the file opened okay, write to it:
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening test.txt");  // if the file didn't open, print an error:
  }
  return 0;
}

/*prints everything from a file*/
bool printfile(const char filepath[]) {
  if (!SD_init) { init_SD(); }
  File myFile = SD.open(filepath);
  if (myFile) {
    Serial.println("test.txt:");
    while (myFile.available()) {  // read from the file until there's nothing else in it:
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("error opening test.txt");  // if the file didn't open, print an error:
  }
  return 1;
}

/*Tests the SD. Work in Progress*/
void testSD() {
  while (!Serial) {}

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
