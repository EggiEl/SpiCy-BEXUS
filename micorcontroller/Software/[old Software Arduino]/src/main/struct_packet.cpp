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
  Serial.print("Size in uC Memory: ");
  Serial.println(sizeof(struct packet));

  Serial.print("id (address: ");
  Serial.print((uintptr_t)&pkt.id, DEC);
  Serial.print("): ");
  Serial.println(pkt.id);

  Serial.print("timestampPacket (address: ");
  Serial.print((uintptr_t)&pkt.timestampPacket, DEC);
  Serial.print("): ");
  Serial.println(pkt.timestampPacket);

  Serial.print("timestampOxy (address: ");
  Serial.print((uintptr_t)&pkt.timestampOxy, DEC);
  Serial.print("): ");
  for (int i = 0; i < 6; i++) {
    Serial.print(pkt.timestampOxy[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("oxigen (address: ");
  Serial.print((uintptr_t)&pkt.oxigen, DEC);
  Serial.print("): ");
  for (int i = 0; i < 6; i++) {
    Serial.print(pkt.oxigen[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("tempTube (address: ");
  Serial.print((uintptr_t)&pkt.tempTube, DEC);
  Serial.print("): ");
  for (int i = 0; i < 6; i++) {
    Serial.print(pkt.tempTube[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("heaterPWM (address: ");
  Serial.print((uintptr_t)&pkt.heaterPWM, DEC);
  Serial.print("): ");
  for (int i = 0; i < 6; i++) {
    Serial.print(pkt.heaterPWM[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("error (address: ");
  Serial.print((uintptr_t)&pkt.error, DEC);
  Serial.print("): ");
  Serial.println(pkt.error);


  Serial.print("tempCpu (address: ");
  Serial.print((uintptr_t)&pkt.tempCpu, DEC);
  Serial.print("): ");
  Serial.println(pkt.tempCpu);

  Serial.print("power (address: ");
  Serial.print((uintptr_t)&pkt.power, DEC);
  Serial.print("): ");
  Serial.print(pkt.power[0]);
  Serial.print(" ");
  Serial.println(pkt.power[1]);

  Serial.print("info (address: ");
  Serial.print((uintptr_t)&pkt.info, DEC);
  Serial.print("): ");
  for (int i = 0; i < 80; i++) {
    Serial.print(pkt.info[i]);
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
    Serial.print("This char was too big for struct packet info: ");
    Serial.println(info);
    return;
  }
  strncpy(data.info, info, sizeof(data.info));
  data.info[sizeof(data.info) - 1] = '\0';  // Ensure null-termination
}
