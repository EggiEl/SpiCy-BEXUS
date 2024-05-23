/*Coordination of the struct packet, wich is a container for downlink purposes*/
#include "header.h"
volatile unsigned long id_struct = 0;
/*return a packet with a timestamp, id and a readout of the CpuTemp*/
struct packet packet_create() {
  struct packet a;
  a.id = id_struct;
  id_struct++;
  a.timestampPacket = millis();
  a.tempCpu = analogReadTemp(3.3f);
  return a;
};

/**
 *  writes a packet to a char* buffer
 * @param struct packet data to write to 
* @return pointer to buffer
*/
char *packettochar(struct packet data) {
  char *buffer = (char *)malloc(sizeof(struct packet));
  memcpy(buffer, &data, sizeof(struct packet));  //converts struct in a char array
  return buffer;
}


/*Prints all avaliable infos about a packet like pointers, size, values and memory*/
void packet_print(struct packet pkt) {
  Serial.println("-----infos about packet----------");
  debug("Size in uC Memory: ");
  Serial.println(sizeof(struct packet));

  debug("id (address: ");
  debug((uintptr_t)&pkt.id, DEC);
  debug("): ");
  Serial.println(pkt.id);

  debug("timestampPacket (address: ");
  debug((uintptr_t)&pkt.timestampPacket, DEC);
  debug("): ");
  Serial.println(pkt.timestampPacket);

  debug("timestampOxy (address: ");
  debug((uintptr_t)&pkt.timestampOxy, DEC);
  debug("): ");
  for (int i = 0; i < 6; i++) {
    debug(pkt.timestampOxy[i]);
    debug(" ");
  }
  Serial.println();

  debug("oxigen (address: ");
  debug((uintptr_t)&pkt.oxigen, DEC);
  debug("): ");
  for (int i = 0; i < 6; i++) {
    debug(pkt.oxigen[i]);
    debug(" ");
  }
  Serial.println();

  debug("tempTube (address: ");
  debug((uintptr_t)&pkt.tempTube, DEC);
  debug("): ");
  for (int i = 0; i < 6; i++) {
    debug(pkt.tempTube[i]);
    debug(" ");
  }
  Serial.println();

  debug("heaterPWM (address: ");
  debug((uintptr_t)&pkt.heaterPWM, DEC);
  debug("): ");
  for (int i = 0; i < 6; i++) {
    debug(pkt.heaterPWM[i]);
    debug(" ");
  }
  Serial.println();

  debug("error (address: ");
  debug((uintptr_t)&pkt.error, DEC);
  debug("): ");
  Serial.println(pkt.error);


  debug("tempCpu (address: ");
  debug((uintptr_t)&pkt.tempCpu, DEC);
  debug("): ");
  Serial.println(pkt.tempCpu);

  debug("power (address: ");
  debug((uintptr_t)&pkt.power, DEC);
  debug("): ");
  debug(pkt.power[0]);
  debug(" ");
  Serial.println(pkt.power[1]);

  debug("info (address: ");
  debug((uintptr_t)&pkt.info, DEC);
  debug("): ");
  for (int i = 0; i < 80; i++) {
    debug(pkt.info[i]);
  }
  Serial.println();
}

/**
 *  writes a char array in the info of an packet
 * @param char* info pointer to a string with carrige return!!!
*/
void packet_writeinfo(struct packet &data, const char *info) {
  unsigned int len = strnlen(info,sizeof(data.info)+1);
  if (len >= sizeof(data.info)) {
    debug("This char was too big for struct packet info: ");
    Serial.println(info);
    return;
  }
  strncpy(data.info, info, sizeof(data.info));
  data.info[sizeof(data.info) - 1] = '\0';  // Ensure null-termination
}
