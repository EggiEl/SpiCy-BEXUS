/*handles the downlink as TCP client*/
#include "header.h"
#include <SPI.h>
#include <Ethernet.h>

char AUTOMATIC_IP_ALLOCATION = 0;
static byte MAC[] =  {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
static IPAddress SERVERIP(169, 254, 218, 4); //(169,254,218,4) (169, 254, 171, 44) (192, 168, 178, 23)// IP address of the PC
// #define SERVERIP IPAddress(100, 81, 57, 236)
static IPAddress CLIENTIP(169, 254, 218, 100); // ip of this uC, leve blank for auto generation IPAddress(169, 168, 1, 100)
// #define DNS IPAddress(8, 8, 8, 8)             // DNS server (e.g., Google DNS)
// #define GATEWAY IPAddress(192, 168, 1, 1)
static IPAddress  SUBNET(255, 255, 0, 0);
static int  SERVERPORT = 8888;
static unsigned int  CONNECTIONTIMEOUT = 3000;

char TCP_init = 0;
static EthernetClient client;

/**
 *  sets the RP2040 as TCP Client for the given ip adress
 * @param TCP_init changes this global variable to true if succesfull
 */
void setup_TCP_Client()
{
  MESSURETIME_START

  debugf_yellow("<setup_TCP_Client>\n");

  SPI.setRX(MISO_LAN);
  SPI.setTX(MOSI_LAN);
  SPI.setSCK(SCK_LAN);

  Ethernet.setRetransmissionCount(8);
  Ethernet.setRetransmissionTimeout(200); // miliseconds
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
    // Ethernet.setLocalIP(CLIENTIP);
  }

  TCP_print_info();

  if (Ethernet.hardwareStatus() && cabletest())
  {
    TCP_init = 1;
    debugf_green("TCP_init_success\n");
  }
  else
  {
    TCP_init = 0;
    debugf_red("TCP_init_failed\n");
  }

  MESSURETIME_STOP
}

/*
 *prints all usefull infos about the TCP client hardware and connection
 */
void TCP_print_info()
{
  debugf("IP Address: %d.%d.%d.%d\n", SERVERIP[0], SERVERIP[1], SERVERIP[2], SERVERIP[3]);
  debugf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
  debugf("-ip_Client: %d.%d.%d.%d\n", Ethernet.localIP()[0], Ethernet.localIP()[1], Ethernet.localIP()[2], Ethernet.localIP()[3]);
  debugf("-dns: %d.%d.%d.%d\n", Ethernet.dnsServerIP()[0], Ethernet.dnsServerIP()[1], Ethernet.dnsServerIP()[2], Ethernet.dnsServerIP()[3]);
  debugf("-gatewayIP: %d.%d.%d.%d\n", Ethernet.gatewayIP()[0], Ethernet.gatewayIP()[1], Ethernet.gatewayIP()[2], Ethernet.gatewayIP()[3]);
  debugf("-subnet: %d.%d.%d.%d\n", Ethernet.subnetMask()[0], Ethernet.subnetMask()[1], Ethernet.subnetMask()[2], Ethernet.subnetMask()[3]);
  debug("-lanIc: ");
  uint8_t hardwareStatus_buff = Ethernet.hardwareStatus();
  switch (hardwareStatus_buff)
  {
  case 0:
    debugf_red("no /faulty Ic\n");
    break;
  case 1:
    debugf_green("W5100\n");
    break;
  case 2:
    debugf_green("W5200\n");
    break;
  case 3:
    debugf_green("W5500\n");
    break;
  default:
    debugf_red("error in code. hardwareStatus() = %i\n", hardwareStatus_buff);
    break;
  }
  uint8_t cabletest_buff = cabletest();
  debug("-cabletest: ");
  switch (cabletest_buff)
  {
  case 0:
    debugf_red("unknown\n");
    break;
  case 1:
    debugf_green("connected\n");
    break;
  case 2:
    debugf_red("not connected\n");
    cabletest_buff = 0;
    break;
  default:
    debugf_red("error in code. cabletest() = %i\n", cabletest_buff);
    cabletest_buff = 0;
    break;
  }
  debugf("-server:");
  if (client.connect(SERVERIP, SERVERPORT))
  {
    debugf_green(" connected\n");
  }
  else
  {
    debugf_red(" not connected\n");
  }
}

/**
 *  Test function for the TCP Server via creating and sending a packet.
 * "Dies ist der Test des Downlinks" is set as info.
 *  Tries for 5 times.
 * @param nPackets: Amount of test packets to send.
 * @param nTries amount of tries to send
 */
void test_TCP_manually(int nPackets, unsigned int nTries)
{
  MESSURETIME_START
  struct packet **packet_buf = (struct packet **)malloc(nPackets * sizeof(struct packet *));
  if (!packet_buf)
  {
    debugf_red("memory allocation failed\n");
    return;
  }

  for (int i = 0; i < nPackets; i++)
  {
    struct packet *newPacket = packet_create();
    packet_writeinfo(newPacket, "Dies ist der Test des Downlinks");
    packet_buf[i] = newPacket;
  }

  char success = send_multible_TCP_packet(packet_buf, nPackets);

  for (int i = 0; i < nPackets; i++)
  {
    free(packet_buf[i]);
  }
  free(packet_buf);

  if (success)
  {
    debugf_green("sendmultible success\n");
  }
  else
  {
    debugf_green("sendmultible failure \n");
  }
  MESSURETIME_STOP
}

/**
 *  Reads in a command out of the TCP Server buffer.
 *  comand structure: [char Command]x3 | [float param1]x2 | [float param2]x2 | [float param3]x2 | [float param4]x2 | ["\n"] end character
 * parameters can be missing, but command and \n must be there
 */
void recieve_TCP_command()
{
  if (!TCP_init)
  {
    setup_TCP_Client();
  }
  if (!client)
  {
    client.connect(SERVERIP, SERVERPORT);
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

  byte *RawByteStream = (byte *)malloc(sizeof(TCPMessageParser));
  if (!RawByteStream)
  {
    debugf_red("Malloc error tcp read RawByteStream\n");
    return;
  }

  signed char status = client.readBytesUntil('\n', RawByteStream, sizeof(TCPMessageParser)); // Returns The next byte (or character), or -1 if none is available.
  if (status == -1)
  {
    debugf_red("Some error parsing tcp readBytesUntil cmmand\n");
    return;
  }
  memcpy(&buffer.ByteStream, RawByteStream, sizeof(TCPMessageParser));
  free(RawByteStream);

  // checks if command is corrupted
  char success = 1;
  if (!(buffer.comand[0] == buffer.comand[1] == buffer.comand[2]))
  {
    debugf_red("TCP Command corrputed\n");
    success = 0;
  }

  // checks if parameter are corrupted
  for (int i = 0; i < 4; i++)
  {
    if (buffer.param[i * 2] = !buffer.param[i * 2 + 1])
    {
      success = 0;
    }
  }

  if (success)
  {
    debugf_green("-TCP command recieved\n");
    handleCommand(buffer.comand[0], buffer.param[0], buffer.param[1], buffer.param[2], buffer.param[3]);
  }
}

/**
 * Sends a array of struct packet via a tcp client
 * remember to free it after!!
 * @param packet pointer to a struct array
 * @param nPackets Number of struct packets in that array
 * @return 1 for success
 */
char send_multible_TCP_packet(struct packet **packet_buff, unsigned int nPackets)
{
  if (!TCP_init)
  {
    setup_TCP_Client();
    if (!TCP_init)
    {
      debugf_red("send_multible_packet|tcp_init failed\n");
      return 0;
    }
  }

  //check if client ready and cnnected
  
  for (unsigned int i = 0; i < nPackets; i++)
  {
    for (int _try = 0; _try < 5; _try++)
    {
      char success = send_TCP_packet(packet_buff[i]);
      if (success)
      {
        break;
      }
      else
      {
        debugf_red("send_multible_TCP couldn´t send package id %i\n", packet_buff[i]->id);
      }
    }
  }
  return 1;
}

/**
 *converts a  packet in a char array and sends this as bitstream to a TCP Server
 *@return 1 for success, -1 for timeout, -2 invalid server, -3 and -4 truncated, -5 memory allocation failed
 *and default case no response from server or hardware issue
 */
char send_TCP_packet(struct packet *packet)
{
  debugf_yellow("<sendpacket-id:%i>", packet->id);
  // packet_print(packet);
  // MESSURETIME_START

  if (!TCP_init)
  {
    setup_TCP_Client();
  }

  char *buffer = packettochar(packet);
  if (buffer == NULL)
  {
    return -5;
  }

  signed char status = 1;
  if (!client.connected())
  { // Whether or not the client is connected. Note that a client is considered connected if the connection has been closed but there is still unread packet.
    debugf_yellow("-connecting_client-");
    status = client.connect(SERVERIP, SERVERPORT);
  }

  switch (status) // client.connect returns different int values depending of the sucess of the operation
  {
  case 1: // Send packet
    if (client.write(buffer, sizeof(struct packet)))
    {
      status = 1;
      debugf_green("sendpacket success\n");
    }
    else
    {
      status = -6;
      debugf_red("sendpacket failed\n");
    }
    // client.flush();                               //waits till all is send //can be left out if TCP Server recives just 200bytes per package.unsave?
    break;
  case -1:
    debugf_red("-sendpacket TIMED_OUT\n");
    break;
  case -2:
    debugf_red("-sendpacket INVALID_SERVER\n");
    break;
  case -3:
    debugf_red("-sendpacket TRUNCATED\n");
    break;
  case -4:
    debugf_red("-sendpacket TRUNCATED\n");
    break;
  default:
    debugf_red("error %i: ", status);
    if (cabletest() == 2)
    {
      debugf_red("cable disconnected\n");
    }
    else
    {
      debugf_red("prob no response from server\n");
    }
    break;
  }

  free(buffer);
  // MESSURETIME_STOP
  return status;
}

/*
Tests the connection via the Ethernit libary.
Returns o if unknown, 1 if sucessfull, 2 if cable not connected
*/
uint8_t cabletest()
{
  return Ethernet.linkStatus();
}

/*Hadware test via a ICMP ping*/
uint8_t ICMP_ping();

/*hosts a Server wich can be acessed via the Ip in local Networks.*/
void testServer()
{
  IPAddress ip(192, 168, 1, 177);
  EthernetServer server(80); // (port 80 is default for HTTP):
  SPI.setRX(MISO_LAN);
  SPI.setTX(MOSI_LAN);
  SPI.setSCK(SCK_LAN);
  Ethernet.init(CS_LAN);

  while (!Serial)
  {
  }
  Serial.println("Ethernet WebServer Example");
  byte MAC[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  Ethernet.begin(MAC); // Ethernet.begin(MAC,ip);
  if (Ethernet.hardwareStatus() == EthernetNoHardware)
  {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true)
    {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF)
  {
    Serial.println("Ethernet cable is not connected.");
  }
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  while (1)
  { // listen for incoming clients
    EthernetClient client = server.available();
    if (client)
    {
      Serial.println("new client");
      // an HTTP request ends with a blank line
      bool currentLineIsBlank = true;
      while (client.connected())
      {
        if (client.available())
        {
          char c = client.read();
          Serial.write(c);
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the HTTP request has ended,
          // so you can send a reply
          if (c == '\n' && currentLineIsBlank)
          {
            // send a standard HTTP response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close"); // the connection will be closed after completion of the response
            client.println("Refresh: 5");        // refresh the page automatically every 5 sec
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            // output the value of each analog input pin
            for (int analogChannel = 0; analogChannel < 6; analogChannel++)
            {
              int sensorReading = analogRead(analogChannel);
              client.print("analog input ");
              client.print(analogChannel);
              client.print(" is ");
              client.print(sensorReading);
              client.println("<br />");
            }
            client.println("</html>");
            break;
          }
          if (c == '\n')
          {
            // you're starting a new line
            currentLineIsBlank = true;
          }
          else if (c != '\r')
          {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
      // give the web browser time to receive the packet
      delay(1);
      // close the connection:
      client.stop();
      Serial.println("client disconnected");
    }
  }
}