/*handles the downlink as TCP client*/
// the downlink is container based, a struct namned packet is used.

#include "header.h"
#include "debug_in_color.h"
#include <SPI.h>
#include <Ethernet_spi0.h> /*The stock Etherent.h libary only supports spi0, i changed that to spi1*/
// #include <Ethernet.h>

static char AUTOMATIC_IP_ALLOCATION = 0;     /*0 means static ip allocation | 1 means dynamic*/
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
  // MESSURETIME_START
#if DEBUG_MODE == 1
  Serial.println("tcp_setup_client>\n");
#endif

  if (TCP_init)
  {
    debugf_warn("tcp_client_already_initialised\n");
    return;
  }

  SPI.setRX(MISO_SPI0);
  SPI.setTX(MOSI_SPI0);
  SPI.setSCK(SCK_SPI0);

  Ethernet.setRetransmissionCount(3);
  Ethernet.setRetransmissionTimeout(20); // miliseconds
  client.setConnectionTimeout(TIMEOUT_TCP_CONNECTION);

  Ethernet.init(CS_LAN);

  if (AUTOMATIC_IP_ALLOCATION)
  {
    //----automatic ip allocation : ----
    if (!Ethernet.begin(MAC))
    {
      debugf_error_notcp("DHCP configuration failed\n");
    }
  }
  else
  {
    //-----manual (static) ip--------------
    Ethernet.begin(MAC, CLIENTIP); // MAC, CLIENTIP, DNS, GATEWAY, SUBNET
    Ethernet.setSubnetMask(SUBNET);
  }

#if DEBUG_MODE == 1
  tcp_print_info();
#endif

  if (Ethernet.hardwareStatus())
  {
    TCP_init = 1;
    debugf_sucess("TCP_init_success\n");
  }
  else
  {
    TCP_init = 0;
    debugf_error_notcp("TCP_init failed\n");
  }
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
  debugf_info("-lanIc: ");
  uint8_t hardwareStatus_buff = Ethernet.hardwareStatus();
  switch (hardwareStatus_buff)
  {
  case 0:
    debugf_error_notcp("no /faulty Ic\n");
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
    debugf_error_notcp("error in code. hardwareStatus() = %i\n", hardwareStatus_buff);
    break;
  }
  uint8_t cabletest_buff = Ethernet.linkStatus();
  debugf_info("-Rj45 Cable: ");
  switch (cabletest_buff)
  {
  case 0:
    debugf_error_notcp("unknown\n");
    break;
  case 1:
    debugf_sucess("connected\n");
    break;
  case 2:
    debugf_error_notcp("not connected\n");
    cabletest_buff = 0;
    break;
  default:
    debugf_error_notcp("error in code. Ethernet.linkStatus() = %i\n", cabletest_buff);
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

  if (!(Ethernet.linkStatus() == 1))
  {
    return;
  }

  if (!client.connected())
  {
    client.setConnectionTimeout(TIMEOUT_TCP_CONNECTION);
    client.connect(SERVERIP, SERVERPORT);
  }

  // checks whether  data is avaliable
  unsigned int nAvalByte = client.available();
  if (!nAvalByte)
  {
    return;
  }

  // reads the TCP_incomming_bytes_buffer in union

  byte ByteStream[35];
  int status = client.readBytes(ByteStream, sizeof(ByteStream)); // Returns the numbers of bytes read, or 0 if no data found.

  if (status == 0)
  {
    error_handler(ERROR_TCP_COMMAND_PARSING);
    debugf_error("Some error parsing tcp readBytesUntil cmmand\n");
    return;
  }

  // for (int i = 0; i < sizeof(ByteStream); i++)
  // {
  //   debugf("0x%02X/", ByteStream[i]);
  // }
  // debugln();

  // convert values
  char command[3];
  command[0] = ByteStream[0];
  command[1] = ByteStream[1];
  command[2] = ByteStream[2];

  float param[8];
  memcpy(param, ByteStream + 3, 8 * sizeof(float));

  debugf_sucess("-TCP command recieved %c|%f|%f|%f|%f\n", command[0], param[0], param[2], param[4], param[6]);

  // checks if command is corrupted
  char success = 1;
  if (!(command[0] == command[1] && command[1] == command[2]))
  {
    error_handler(ERROR_TCP_COMMAND_CORRUPT);
    debugf_error("TCP Command corrputed\n");
    debugf_error("Command: %c %c %c\n Param:\n", command[0], command[1], command[2]);
    for (uint8_t i = 0; i < 4; i++)
    {
      debugf_error("%f ", param[i * 2]);
      debugf_error("%f\n", param[i * 2 + 1]);
    }
    success = 0;
  }

  // checks if parameter are corrupted
  for (int i = 0; i < 4; i++)
  {
    if (param[i * 2] != param[i * 2 + 1])
    {
      error_handler(ERROR_TCP_PARAM_CORRUPT);
      debugf_error("TCP parameters corrputed\n");
      success = 0;
    }
  }

  if (success)
  {
    handle_command(command[0], param[0], param[2], param[4], param[6]);
  }
}

/**
 *converts a  packet in a char array and sends this as bitstream to a TCP Server
 *@return 1 for success, -1 for timeout, -2 invalid server, -3 and -4 truncated, -5 memory allocation failed, -6 for no rj cable
 *and default case no response from server or hardware issue
 */
char tcp_send_packet(struct packet *packet)
{
  // debugf_status("sendpacket-id:%i", packet->id);
  if (!TCP_init)
  {
    tcp_setup_client();
  }

  if (!(Ethernet.linkStatus() == 1))
  {
    return (char)-6;
  }

  char buffer[sizeof(struct packet)];
  packettochar(packet, buffer);

 int status = 1;
  if (!client.connected())
  { // Whether or not the client is connected. Note that a client is considered connected if the connection has been closed but there is still unread packet.
    client.setConnectionTimeout(TIMEOUT_TCP_CONNECTION);
    status = client.connect(SERVERIP, SERVERPORT);
  }

  switch (status) // client.connect returns different int values depending of the sucess of the operation
  {
  case 1: // Send packet
    if (client.write(buffer, sizeof(struct packet)))
    {
      status = 1;
      // debugf_sucess(" success\n");
    }
    else
    {
      status = -6;
      debugf_error_notcp("send_packet failed\n");
    }
    // client.flush();                               //waits till all is send //can be left out if TCP Server recives just 200bytes per package.unsave?
    break;
  case -1:
    debugf_error_notcp(" timeout\n");
    break;
  case -2:
    debugf_error_notcp(" invalid server\n");
    break;
  case -3:
    debugf_error_notcp(" truncated\n");
    break;
  case -4:
    debugf_error_notcp(" truncated_2\n");
    break;
  default:
    debugf_error_notcp("send tcp error %i: ", status);
    if (Ethernet.linkStatus() == 2)
    {
      debugf_error_notcp(" cable disconnected\n");
    }
    else
    {
      debugf_error_notcp(" no response\n");
    }
    break;
  }
  return status;
}

/**
 * Sends a array of struct packet via a tcp client
 * remember to free it after!!
 * @param packet pointer to a struct array
 * @param nPackets number of struct packets in that array
 * @return 1 for success
 */
void tcp_send_multible_packets(struct packet **packet_buff, unsigned int nPackets)
{
  if (!TCP_init)
  {
    tcp_setup_client();
  }

  if (!(Ethernet.linkStatus() == 1))
  {
    return;
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
        error_handler(ERROR_TCP_SEND_MULTIBLE_FAILED, ERROR_DESTINATION_NO_TCP);
      }
    }
  }
}

/**
 * Sends a TCP command. [4294967296 unsigned int] [errorcode char][errorcode char] ["\0"]
 * It allways starts with a 4294967296,
 *  that is do different it from a packet wich starts with an unsigned long id.
 * 4294967296 would be the last id before a overflow
 *
 * */
void tpc_send_error(const unsigned char error)
{
  // debugf_status("sender_down_error:%u", error);
  if (!TCP_init)
  {
    tcp_setup_client();
  }

  uint8_t sizebuffer = 7 * (sizeof(char));
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

  int status = 1;
  if (!client.connected())
  { // Whether or not the client is connected. Note that a client is considered connected if the connection has been closed but there is still unread packet.
    client.setConnectionTimeout(TIMEOUT_TCP_CONNECTION);
    status = client.connect(SERVERIP, SERVERPORT);
  }

  if (status)
  {
    client.write(buffer, sizebuffer);
  }
}

void tpc_send_string(const char string[])
{
  if (!TCP_init)
  {
    tcp_setup_client();
  }

  if (!(Ethernet.linkStatus() == 1))
  {
    return;
  }

  int status = 1;
  if (!client.connected())
  { // Whether or not the client is connected. Note that a client is considered connected if the connection has been closed but there is still unread packet.
    client.setConnectionTimeout(TIMEOUT_TCP_CONNECTION);
    status = client.connect(SERVERIP, SERVERPORT);
  }

  if (status)
  {
    client.write(string, strnlen(string, 1000));
    // client.flush();
  }
}

/**
 * acts like a printf with all the same parameters,
 * just sends sting to a tcp server not console.
 */
void tcp_sendf(const char *__restrict format, ...)
{
  if (!(Ethernet.linkStatus() == 1))
  {
    return;
  }

  // TODO testing on memory savety
  va_list args;
  va_list args_copy;
  char *buffer;
  int buffer_size;

  // Start processing the variable arguments
  va_start(args, format);

  // Copy args to use it twice
  va_copy(args_copy, args);

  // Get the size of the buffer needed
  buffer_size = vsnprintf(NULL, 0, format, args) + 1; // +1 for the null terminator

  // Allocate the buffer dynamically
  buffer = (char *)malloc(buffer_size);
  if (buffer == NULL)
  // Handle memory allocation failure
  {
    error_handler(ERROR_TCP_DEBUG_MEMORY, ERROR_DESTINATION_NO_TCP);
    debugf_error_notcp("memory allocation failed tcp_sendf\n");
    va_end(args);
    va_end(args_copy);
    return;
  }

  // Format the string
  vsnprintf(buffer, buffer_size, format, args_copy);

  // Send the formatted string over TCP
  tpc_send_string(buffer);

  // Clean up
  free(buffer);
  va_end(args);
  va_end(args_copy);
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
  /*creates packet buffer*/
  struct packet **packet_buf = (struct packet **)malloc(nPackets * sizeof(struct packet *));
  if (!packet_buf)
  {
    error_handler(ERROR_TCP_DEBUG_MEMORY, ERROR_DESTINATION_NO_TCP);
    return;
  }

  /*fills packet buffer with packets*/
  for (int i = 0; i < nPackets; i++)
  {
    struct packet *newPacket = packet_create();
    packet_buf[i] = newPacket;
  }

  /*send buffer*/
  tcp_send_multible_packets(packet_buf, nPackets);

  /*frees packet buffer*/
  for (int i = 0; i < nPackets; i++)
  {
    destroy_packet(packet_buf[i]);
  }
  free(packet_buf);
}

/*returns the Ethernet-linkStatus() for the ethernet.h libary*/
unsigned char tcp_link_status()
{
  return Ethernet.linkStatus();
}

#include <LittleFS.h>
#include <picoOTA.h>
/**
 * @param size_firmware Size of over the air recieved firmware update. Used to check data integraty at least a bit.
 */
void tcp_receive_OTA_update(int size_firmware)
{
  const char filename[] = "/firmware.bin"; // before: blink.bin.gz
  debugf_status("Over the air firmware update.\n");

  /*creates file*/
  if (!LittleFS.begin())
  {
    debugf_red("An error has occurred while mounting LittleFS\n");
    return;
  }

  File f = LittleFS.open(filename, "w");
  if (!f)
  {
    debugf_red("Failed to open file for writing\n");
    LittleFS.end();
    return;
  }

  /*connects client*/
  if (!TCP_init)
  {
    tcp_setup_client();
  }

  if (!client.connected())
  {
    client.connect(SERVERIP, SERVERPORT);
    client.setConnectionTimeout(TIMEOUT_TCP_CONNECTION);
  }

  /*checks if data avaliable*/
  debugf_info("Waiting 7 seconds for server connection...\n");
  rp2040.wdt_begin(8000);
  unsigned long timestamp = millis() + 7000;
  while (!client.available())
  {
    if (millis() > timestamp)
    {
      debugf_red("No firmware uplad avaliable. Start firmware upload on the groundstation before calling this method.\n");
      rp2040.wdt_begin(TIMEOUT_WATCHDOG);
      f.close();
      LittleFS.end();
      return;
    }
  }

  /*recieving file*/
  debugf_status("receiving OTA image...\n");
  rp2040.wdt_reset();
  while (client.available())
  {
    int byteRead = client.read(); // returns -1 if none is available.
    if (byteRead >= 0)
    {
      f.write((uint8_t)byteRead);
    }
    else
    {
      break;
    }
  }
  debugf_sucess("File size of %zu received and saved\n", f.size());

  /*checks if size of recieved file checks out*/
  if (f.size() != size_firmware)
  {
    debugf_red("Size of recieved data:%zu doesn´t match size the firmware should have:%d", f.size(), size_firmware);
    rp2040.wdt_begin(TIMEOUT_WATCHDOG);
    f.close();
    return;
  }
  debugf_sucess("Size of recieved data:%zu matchs the size the firmware should have:%d\n", f.size(), size_firmware);
  f.close();

  /*perform OTA update*/
  debugf_info("Programming OTA commands...\n");
  rp2040.wdt_reset();
  picoOTA.begin();
  rp2040.wdt_reset();
  if (!picoOTA.addFile(filename))
  {
    debugf_red("error adding firmware to OTA files.\n");
    LittleFS.end();
    return;
  }
  rp2040.wdt_reset();
  if (!picoOTA.commit())
  {
    debugf_red("error ota commiting firmware.\n");
    LittleFS.end();
    return;
  }
  LittleFS.end();
  debugf_info("OTA update completed.\n");

  // /*rebooting*/
  debugf_status("Rebooting...\n");
  delay(5000);
  rp2040.restart();
}
