
/*Coordination of the struct packet, wich is a container for downlink purposes*/
#include "header.h"

static unsigned long id_struct = 0;

/**
 * @return a packet with a timestamp,
 * id and a readout of the CpuTemp
 * */
struct packet *packet_create()
{
  struct packet *a = (struct packet *)calloc(1, sizeof(struct packet));
  if (a != NULL)
  {
    if (id_struct < 4294967296) // if y send something starting down with a 4294967296, its a error. So to not loose data, there can#t be a packet with that id.
    {
      a->id = id_struct;
    }
    else
    {
      id_struct = 0;
      a->id = id_struct;
    }

    id_struct++;
    a->timestampPacket = millis();
  }
  else
  {
    debugf_error("packet_create memory alloc failed\n");
  }
  return a;
}

/**
 * frees the memory of a packet
 * and sets its pointer to NULL.
 * if it was already NULL,
 * does nothing
 * */
void destroy_packet(struct packet *p)
{
  if (p != NULL)
  {
    free(p);
    p = NULL;
  }
  else
  {
    debugf_warn("tried to free packet twice\n");
    ;
  }
}

/**
 *  writes a packet to a char* buffer
 * remember to free!!
 * @param struct packet data to write to
 * @return pointer to buffer
 */
char *packettochar(const struct packet *data)
{
  char *buffer = (char *)malloc(sizeof(struct packet));
  if (!buffer)
  {
    debugf_error("packettochar Memory allocation failed\n");
    return NULL;
  }
  memcpy(buffer, data, sizeof(struct packet)); // Copy struct data into buffer
  return buffer;
}

/*Prints all avaliable infos about a packet like pointers, size, values and memory*/
void packet_print(struct packetold *pkt)
{

  debugf_status("-----infos about packet----------\n");

  debug("Size in uC Memory: ");
  debugln(sizeof(struct packet));

  debug("id (address: ");
  debug((uintptr_t)&pkt->id, DEC);
  debug("): ");
  debugln(pkt->id);

  debug("timestampPacket (address: ");
  debug((uintptr_t)&pkt->timestampPacket, DEC);
  debug("): ");
  debugln(pkt->timestampPacket);

  debug("timestampOxy (address: ");
  debug((uintptr_t)&pkt->timestampOxy, DEC);
  debug("): ");
  for (int i = 0; i < 6; i++)
  {
    debug(pkt->timestampOxy[i]);
    debug(" ");
  }
  debugln();

  debug("oxigen (address: ");
  debug((uintptr_t)&pkt->oxigen, DEC);
  debug("): ");
  for (int i = 0; i < 6; i++)
  {
    debug(pkt->oxigen[i]);
    debug(" ");
  }
  debugln();

  debug("tempTube (address: ");
  debug((uintptr_t)&pkt->tempTube, DEC);
  debug("): ");
  for (int i = 0; i < 6; i++)
  {
    debug(pkt->tempTube[i]);
    debug(" ");
  }
  debugln();

  debug("heaterPWM (address: ");
  debug((uintptr_t)&pkt->heaterPWM, DEC);
  debug("): ");
  for (int i = 0; i < 6; i++)
  {
    debug(pkt->heaterPWM[i]);
    debug(" ");
  }
  debugln();

  debug("error (address: ");
  debug((uintptr_t)&pkt->error, DEC);
  debug("): ");
  debugln(pkt->error);

  debug("tempCpu (address: ");
  debug((uintptr_t)&pkt->tempCpu, DEC);
  debug("): ");
  debugln(pkt->tempCpu);

  debug("power (address: ");
  debug((uintptr_t)&pkt->power, DEC);
  debug("): ");
  debug(pkt->power[0]);
  debug(" ");
  debugln(pkt->power[1]);

  debug("info (address: ");
  debug((uintptr_t)&pkt->info, DEC);
  debug("): ");
  for (int i = 0; i < 80; i++)
  {
    debug(pkt->info[i]);
  }
  debugln();
}

/**
 *  writes a char array in the info of an packet
 * @param char* info pointer to a string with carrige return!!!
 */
// void packet_write_error(struct packet *packet, const char *error_new)
// {
//   size_t len = strnlen(error_new, sizeof(packet->errors));
//   if (len >= sizeof(packet->errors))
//   {
//     debugf_error("This char was too big for struct packet info: %s\n", error_new);
//     return;
//   }
//   strncpy(packet->errors, error_new, len);
//   packet->errors[len] = '\0'; // Ensure null-termination
// }