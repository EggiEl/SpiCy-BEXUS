/*handles the downlink as TCP client*/
// the downlink is container based, a struct namned packet is used.

#include "header.h"
#include "debug_in_color.h"
#include <SPI.h>
// #include <Ethernet_spi1.h> /*The stock Etherent.h libary only supports spi0, i changed that to spi1*/
#include <Ethernet.h>

char AUTOMATIC_IP_ALLOCATION = 0;            /*0 means static ip allocation | 1 means dynamic*/
static IPAddress SERVERIP(169, 254, 218, 4); // IP address of the Groundstation
// #define SERVERIP IPAddress(100, 81, 57, 236)
static IPAddress CLIENTIP(169, 254, 218, 100); // ip of this uC. Used ony in fixed IP allocation
// #define DNS IPAddress(8, 8, 8, 8)             // DNS server (e.g., Google DNS)
// #define GATEWAY IPAddress(192, 168, 1, 1)
static IPAddress SUBNET(255, 255, 0, 0);
static int SERVERPORT = 8888;
static byte MAC[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

volatile char TCP_init = 0; /*If true the Ethernet Ic is connected and fuctional*/
EthernetClient client;

/**
 *  sets the RP2040 as TCP Client for the given ip adress
 * @param TCP_init changes this global variable to true if succesfull
 */
void tcp_setup_client()
{
  MESSURETIME_START

  debugf_status("tcp_setup_client>\n");

  // SPI.setRX(MISO_SPI0);
  // SPI.setTX(MOSI_SPI0);
  // SPI.setSCK(SCK_SPI0);

  SPI.setRX(LAN_MISO);
  SPI.setTX(LAN_MOSI);
  SPI.setSCK(LAN_SCK);

  Ethernet.setRetransmissionCount(3);
  Ethernet.setRetransmissionTimeout(20); // miliseconds
  client.setConnectionTimeout(CONNECTIONTIMEOUT);

  Ethernet.init(CS_LAN);

  if (AUTOMATIC_IP_ALLOCATION)
  {
    //----automatic ip allocation : ----
    if (!Ethernet.begin(MAC))
    {
      debug("DHCP configuration failed}-");
      debugln("-fail}-");
      return;
    }
  }
  else
  {
    //-----manual (static) ip--------------
    Ethernet.begin(MAC, CLIENTIP); // MAC, CLIENTIP, DNS, GATEWAY, SUBNET
    Ethernet.setSubnetMask(SUBNET);
  }

  tcp_print_info();

  if (Ethernet.hardwareStatus())
  {
    TCP_init = 1;
    debugf_sucess("TCP_init_success\n");
  }
  else
  {
    TCP_init = 0;
    error_handler(ERROR_TCP_INI,ERROR_DESTINATION_NO_TCP);
    debugf_error("TCP_init_failed\n");
  }

  MESSURETIME_STOP
}

/*
 *prints all usefull infos about the TCP client hardware and connection
 */
void tcp_print_info()
{
  debugf_info("IP Address: %d.%d.%d.%d\n", SERVERIP[0], SERVERIP[1], SERVERIP[2], SERVERIP[3]);
  debugf_info("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
  debugf_info("-ip_Client: %d.%d.%d.%d\n", Ethernet.localIP()[0], Ethernet.localIP()[1], Ethernet.localIP()[2], Ethernet.localIP()[3]);
  debugf_info("-dns: %d.%d.%d.%d\n", Ethernet.dnsServerIP()[0], Ethernet.dnsServerIP()[1], Ethernet.dnsServerIP()[2], Ethernet.dnsServerIP()[3]);
  debugf_info("-gatewayIP: %d.%d.%d.%d\n", Ethernet.gatewayIP()[0], Ethernet.gatewayIP()[1], Ethernet.gatewayIP()[2], Ethernet.gatewayIP()[3]);
  debugf_info("-subnet: %d.%d.%d.%d\n", Ethernet.subnetMask()[0], Ethernet.subnetMask()[1], Ethernet.subnetMask()[2], Ethernet.subnetMask()[3]);
  debug("-lanIc: ");
  uint8_t hardwareStatus_buff = Ethernet.hardwareStatus();
  switch (hardwareStatus_buff)
  {
  case 0:
    debugf_error("no /faulty Ic\n");
    break;
  case 1:
    debugf_sucess("W5100\n");
    break;
  case 2:
    debugf_sucess("W5200\n");
    break;
  case 3:
    debugf_sucess("W5500\n");
    break;
  default:
    debugf_error("error in code. hardwareStatus() = %i\n", hardwareStatus_buff);
    break;
  }
  uint8_t cabletest_buff = Ethernet.linkStatus();
  debug("-Rj45 Cable: ");
  switch (cabletest_buff)
  {
  case 0:
    debugf_error("unknown\n");
    break;
  case 1:
    debugf_sucess("connected\n");
    break;
  case 2:
    debugf_error("not connected\n");
    cabletest_buff = 0;
    break;
  default:
    debugf_error("error in code. Ethernet.linkStatus() = %i\n", cabletest_buff);
    cabletest_buff = 0;
    break;
  }

  if (cabletest_buff && hardwareStatus_buff)
  {
    debugf_info("-server: ");
    if (client.connect(SERVERIP, SERVERPORT))
    {
      debugf_sucess("connected\n");
    }
    else
    {
      debugf_warn("not connected\n");
    }
  }
  else
  {
    debugf_warn("no servertest becuase of faulty cable/Ic\n");
  }
}

/**
 *  Reads in a command out of the TCP Server buffer and calls it if valid via commandHandler
 *  comand structure: [char Command]x3 | [float param1]x2 | [float param2]x2 | [float param3]x2 | [float param4]x2 | ["\n"] end character
 * parameters can be missing, but command and \n must be there
 */
void tcp_check_command()
{
  if (!TCP_init)
  {
    tcp_setup_client();
  }

  if (!client.connected())
  {
    client.connect(SERVERIP, SERVERPORT);
    client.setConnectionTimeout(CONNECTIONTIMEOUT);
  }

  // checks whether  data is avaliable
  unsigned int nAvalByte = client.available();
  if (!nAvalByte)
  {
    return;
  }

  // reads the TCP_incomming_bytes_buffer in union
  union TCPMessageParser
  {
    byte ByteStream[35];
    struct
    {
      char comand[3];
      float param[8];
    };
  } buffer;

  int status = client.readBytesUntil('\n', buffer.ByteStream, sizeof(TCPMessageParser)); // Returns The next byte (or character), or -1 if none is available.
  if (status == -1)
  {
    error_handler(ERROR_TCP_COMMAND_PARSING);
    debugf_error("Some error parsing tcp readBytesUntil cmmand\n");
    return;
  }

  /*old kinda working code:
  // reads the TCP_incomming_bytes_buffer in union
    union TCPMessageParser
    {
      byte ByteStream[35];
      struct
      {
        char comand[3];
        float param[8];
      };
    } buffer;

    byte *RawByteStream = (byte *)malloc(sizeof(TCPMessageParser));
    if (!RawByteStream)
    {
      debugf_error("Malloc error tcp read RawByteStream\n");
      return;
    }

    int status = client.readBytesUntil('\n', RawByteStream, sizeof(TCPMessageParser)); // Returns The next byte (or character), or -1 if none is available.
    if (status == -1)
    {
      debugf_error("Some error parsing tcp readBytesUntil cmmand\n");
      free_ifnotnull(RawByteStream);
      return;
    }
    memcpy(&buffer.ByteStream, RawByteStream, sizeof(TCPMessageParser));
    free_ifnotnull(RawByteStream);*/

  // checks if command is corrupted
  char success = 1;
  if (!(buffer.comand[0] == buffer.comand[1] && buffer.comand[1] == buffer.comand[2]))
  {
    error_handler(ERROR_TCP_COMMAND_CORRUPT);
    debugf_error("TCP Command corrputed\n");
    debugf_error("Command:\n%c\n%c\n%c\n", buffer.comand[0], buffer.comand[1], buffer.comand[2]);
    for (uint8_t i = 0; i < 4; i++)
    {
      debugf_error("%f\n", buffer.param[i * 2]);
      debugf_error("%f\n", buffer.param[i * 2 + 1]);
    }
    success = 0;
  }

  // checks if parameter are corrupted
  for (int i = 0; i < 4; i++)
  {
    if (buffer.param[i * 2] != buffer.param[i * 2 + 1])
    {
      error_handler(ERROR_TCP_PARAM_CORRUPT);
      success = 0;
    }
  }

  if (success)
  {
    debugf_sucess("-TCP command recieved %c|%f|%f|%f|%f\n", buffer.comand[0], buffer.param[0], buffer.param[1], buffer.param[2], buffer.param[3]);
    handleCommand(buffer.comand[0], buffer.param[0], buffer.param[1], buffer.param[2], buffer.param[3]);
  }
}

/**
 *converts a  packet in a char array and sends this as bitstream to a TCP Server
 *@return 1 for success, -1 for timeout, -2 invalid server, -3 and -4 truncated, -5 memory allocation failed
 *and default case no response from server or hardware issue
 */
char tcp_send_packet(struct packet *packet)
{
  debugf_status("sendpacket-id:%i", packet->id);
  // packet_print(packet);
  // MESSURETIME_START

  if (!TCP_init)
  {
    tcp_setup_client();
  }
  char buffer[sizeof(struct packet)];
  packettochar(packet, buffer);

  signed char status = 1;
  if (!client.connected())
  { // Whether or not the client is connected. Note that a client is considered connected if the connection has been closed but there is still unread packet.
    debugf_status("-connecting_client-");
    status = client.connect(SERVERIP, SERVERPORT);
    client.setConnectionTimeout(CONNECTIONTIMEOUT);
  }

  switch (status) // client.connect returns different int values depending of the sucess of the operation
  {
  case 1: // Send packet
    if (client.write(buffer, sizeof(struct packet)))
    {
      status = 1;
      debugf_sucess("sendpacket success\n");
    }
    else
    {
      status = -6;
      error_handler(ERROR_TCP_SEND_FAILED,ERROR_DESTINATION_NO_TCP);
      debugf_error("sendpacket failed\n");
    }
    // client.flush();                               //waits till all is send //can be left out if TCP Server recives just 200bytes per package.unsave?
    break;
  case -1:
    error_handler(ERROR_TCP_SEND_TIMEOUT,ERROR_DESTINATION_NO_TCP);
    debugf_error("-sendpacket TIMED_OUT\n");
    break;
  case -2:
  error_handler(ERROR_TCP_SERVER_INVALID,ERROR_DESTINATION_NO_TCP);
    debugf_error("-sendpacket INVALID_SERVER\n");
    break;
  case -3:
  error_handler(ERROR_TCP_TRUNCATED,ERROR_DESTINATION_NO_TCP);
    debugf_error("-sendpacket TRUNCATED\n");
    break;
  case -4:
  error_handler(ERROR_TCP_TRUNCATED_2,ERROR_DESTINATION_NO_TCP);
    debugf_error("-sendpacket TRUNCATED\n");
    break;
  default:
    debugf_error("error %i: ", status);
    if (Ethernet.linkStatus() == 2)
    {
      error_handler(ERROR_TCP_CABLE_DISCO,ERROR_DESTINATION_NO_TCP);
      debugf_error("cable disconnected\n");
    }
    else
    {
      error_handler(ERROR_TCP_NO_RESPONSE,ERROR_DESTINATION_NO_TCP);
      debugf_error("prob no response from server\n");
    }
    break;
  }

  // MESSURETIME_STOP
  return status;
}

/**
 * Sends a array of struct packet via a tcp client
 * remember to free it after!!
 * @param packet pointer to a struct array
 * @param nPackets number of struct packets in that array
 * @return 1 for success
 */
char tcp_send_multible_packets(struct packet **packet_buff, unsigned int nPackets)
{
  if (!TCP_init)
  {
    tcp_setup_client();
    if (!TCP_init)
    {
      debugf_error("send_multible_packet|tcp_init failed\n");
      return 0;
    }
  }

  // check if client ready and cnnected

  for (unsigned int i = 0; i < nPackets; i++)
  {
    for (int _try = 0; _try < 5; _try++)
    {
      char success = tcp_send_packet(packet_buff[i]);
      if (success)
      {
        break;
      }
      else
      {
        debugf_error("send_multible_TCP couldn´t send package id %i\n", packet_buff[i]->id);
      }
    }
  }
  return 1;
}

/**
 * Sends a TCP command. [4294967296 unsigned int] [errorcode char][errorcode char] ["\0"]
 * It allways starts with a 4294967296,
 *  that is do different it from a packet wich starts with an unsigned long id.
 * 4294967296 would be the last id before a overflow
 *
 * */
void tpc_send_error(unsigned char error)
{
  debugf_status("sender_down_error:%u", error);

  if (!TCP_init)
  {
    tcp_setup_client();
  }

  uint8_t sizebuffer = sizeof(unsigned int) + 4 * (sizeof(char));
  char *buffer = (char *)calloc(sizebuffer, 1);
  if (buffer == NULL)
  {
    return;
  }

  buffer[0] = 0b11111111;
  buffer[1] = 0b11111111;
  buffer[2] = 0b11111111;
  buffer[3] = 0b11111111;
  buffer[4] = error;
  buffer[5] = error;
  buffer[6] = '\0';

  if (!client.connected())
  { // Whether or not the client is connected. Note that a client is considered connected if the connection has been closed but there is still unread packet.
    debugf_status("-connecting_client-");
  }
  uint8_t status = client.write(buffer, sizebuffer);
  if (status)
  {
    debugf_info("successs\n");
  }
  else
  {
    debugf_info("faliue\n");
  }

  free_ifnotnull(buffer);
}

/**
 *  Test function for the TCP Server via creating and sending a packet.
 * "Dies ist der Test des Downlinks" is set as info.
 *  Tries for 5 times.
 * @param nPackets: Amount of test packets to send.
 * @param nTries amount of tries to send
 */
void tpc_testmanually(int nPackets, unsigned int nTries)
{
  MESSURETIME_START
  struct packet **packet_buf = (struct packet **)malloc(nPackets * sizeof(struct packet *));
  if (!packet_buf)
  {
    debugf_error("memory allocation failed\n");
    return;
  }

  for (int i = 0; i < nPackets; i++)
  {
    struct packet *newPacket = packet_create();
    // packet_write_error(newPacket, "Dies ist der Test des Downlinks");
    packet_buf[i] = newPacket;
  }

  char success = tcp_send_multible_packets(packet_buf, nPackets);

  for (int i = 0; i < nPackets; i++)
  {
    destroy_packet(packet_buf[i]);
  }
  free_ifnotnull(packet_buf);

  if (success)
  {
    debugf_sucess("sendmultible success\n");
  }
  else
  {
    debugf_sucess("sendmultible failure \n");
  }
  MESSURETIME_STOP
}

unsigned char tcp_link_status()
{
  return Ethernet.linkStatus();
}