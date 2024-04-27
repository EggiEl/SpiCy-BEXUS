/*handles the downlink as TCP client*/
#include "header.h"
#include <SPI.h>
#include <Ethernet.h>

char AUTOMATIC_IP_ALLOCATION = 0;

#define SPI_FREQ_LAN 12MHz // not used and no need to change
#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
#define SERVERIP IPAddress(169, 254, 218, 4) //(169,254,218,4) (169, 254, 171, 44) (192, 168, 178, 23)// IP address of the PC
// #define SERVERIP IPAddress(100, 81, 57, 236)
#define CLIENTIP IPAddress(169, 254, 218, 100) // ip of this uC, leve blank for auto generation IPAddress(169, 168, 1, 100)
// #define DNS IPAddress(8, 8, 8, 8)             // DNS server (e.g., Google DNS)
// #define GATEWAY IPAddress(192, 168, 1, 1)
#define SUBNET IPAddress(255, 255, 0, 0)
#define SERVERPORT 8888
#define CONNECTIONTIMEOUT 10000

char TCP_init = 0;
EthernetClient client;

struct test
{
  int a = 5;
  float b = 4.4;
} testa;

/**
 *  sets the RP2040 as TCP Client for the given ip adress
 * @param TCP_init changes this global variable to true if succesfull
 */
void setup_TCP_Client()
{
  debugf_yellow("<setup_TCP_Client>\n");

  MESSURETIME_START

  byte mac[] = MAC; // MAC address
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
    if (!Ethernet.begin(mac))
    {
      debug("DHCP configuration failed}-");
      debugln("-fail}-");
      return;
    }
  }
  else
  {
    //-----manual (static) ip--------------
    Ethernet.begin(mac, CLIENTIP); // mac, CLIENTIP, DNS, GATEWAY, SUBNET
    Ethernet.setSubnetMask(SUBNET);
    // Ethernet.setLocalIP(CLIENTIP);
  }

  //--------debugg prints--------------
  debug("-ip_Server: ");
  debugln(SERVERIP);

  debug("-mac: ");
  uint8_t *mac_address = (uint8_t *)malloc(8);
  Ethernet.MACAddress(mac_address);
  for (uint8_t i = 0; i < 6; i++)
  {
    if (DEBUG)
    {
      Serial.print(mac_address[i], HEX);
    }
    debug(" ");
  }
  free(mac_address);
  debugln();

  debug("-ip_Client: ");
  debugln(Ethernet.localIP());

  debug("-dns: ");
  debugln(Ethernet.dnsServerIP());

  debug("-gatewayIP: ");
  debugln(Ethernet.gatewayIP());

  debug("-subnet: ");
  debugln(Ethernet.subnetMask());

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

  // for (int i = 0; i < 1000; i++) { dosen´t work for some reason?!?!
  //   if (client) {
  //     TCP_init = 1;
  //     break;
  //   }
  // }
  if (hardwareStatus_buff && cabletest_buff)
  {
    while (!client)
    {
    }
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

/**
 *  Test function for the TCP Server via creating and sending a packet.
 * with "Dies ist der Test der des structs Packet" as info.
 *  Tries for 5 times.
 */
void test_TCP_manually()
{
  // packet send
  struct packet data;
  data = packet_create();
  data.oxigen[2] = 200;
  data.oxigen[1] = 100;
  packet_writeinfo(data, "Dies ist der Test der des structs Packet");
  // packet_print(data);
  data.timestampPacket = millis();
  int n = 5;
  while (n > 0)
  {
    if (send_TCP_packet(data) == 1)
    {
      n = 0;
    }
    else
    {
      n--;
    }
  }
  // delay(500);
}

/**
 *  Reads in commands via the TCP Server connection.
 */
void recieve_TCP_command()
{
  debug("-{recieve_TCPdTCPpacket-id:");

  if (!TCP_init)
  {
    setup_TCP_Client();
  }

  if (!client)
  {
    client.connect(SERVERIP, SERVERPORT);
  }

  // reading out the TCP buffer
  int size = client.available();

  if (!size)
  {
    debugln("-noavaliable}-");
    return;
  }

  char *buffer = (char *)malloc(size + 1);
  for (int i = 0; i < size; i++)
  {
    signed char data = client.read(); // Returns The next byte (or character), or -1 if none is available.
    if (data == -1)
    {
      debug("-client.read()=-1-");
      break;
    }
    buffer[i] = data;
  }
  buffer[size + 1] = '\n';
  // reading out command
  int comand_str = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);

  switch (comand_str)
  {
  case ('s' + ('r' << 8) + ('o' << 16)):
  {
    debugln("-command 'sro' recieved-");
    uint32_t status_a = get_Status();
    char buf[5]; // Array to hold the content of status
    memcpy(buf, &status_a, sizeof(status_a));
    buf[4] = '\0';
    // packet_writeinfo(buf);
    break;
  }
  default:
  {
    debug("-unknown command ");
    debug(comand_str);
    debugln("recieved-");
    break;
  }
  }

  debugln("-end}-");
}

/**
 * Sends one
 */
int send_multible_TCP_packet(struct packet *data, unsigned int nPackets)
{
  for (unsigned int i = 0; i < nPackets; i++)
  {
  }
  return 0;
}

/**
 *converts a  packet in a char array and sends this as bitstream to a TCP Server
 *@return 1 for success, -1 for timeout, -2 invalid server, .3 and -4 truncated
 *and default case no response from server or hardware issue
 */
int send_TCP_packet(struct packet data)
{
  debugf_yellow("<sendpacket-id:%i>", data.id);
  MESSURETIME_START

  if (!TCP_init)
  {
    setup_TCP_Client();
  }

  char *buffer = (char *)malloc(sizeof(struct packet)); // converts struct in a char array
  memcpy(buffer, &data, sizeof(struct packet));

  int status = 1;
  // needs work when sending is implemented
  //  if (client.available()) { status = client.connect(SERVERIP, SERVERPORT); }

  if (!client.connected())
  { // Whether or not the client is connected. Note that a client is considered connected if the connection has been closed but there is still unread data.
    debugf_yellow("-connecting_client-");
    status = client.connect(SERVERIP, SERVERPORT);
  }

  switch (status)
  { // client.connect returns different int values depending of the sucess of the operation
  case 1:
    client.write(buffer, sizeof(struct packet)); // Send data
    // client.flush();                               //waits till all is send //can be left out if TCP Server recives just 200bytes per package.unsave?
    debugf_green("sendpacket success");
    break;
  case -1:

    debugf_red("-sendpacket TIMED_OUT");
    break;
  case -2:
    debugf_red("-sendpacket INVALID_SERVER");
    break;
  case -3:
    debugf_red("-sendpacket TRUNCATED");
    break;
  case -4:
    debugf_red("-sendpacket TRUNCATED");
    break;
  default:
    debug("Case: ");
    debug(status);
    if (cabletest()==2)
    {
      debugf_red("-sendpacket: cable disconnected");
    }
    else
    {
      debugf_red("-sendpacket: no response from server");
    }
    break;
  }
  MESSURETIME_STOP
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

// sends an dynamic char array as an TCP client
void send_TCP(char *data, unsigned long int size)
{
  if (size == 0)
  {
    Serial.println("Error: Invalid size. Size must be greater than 0.");
    return;
  }
  if (client.connect(SERVERIP, 8888))
  {
    for (unsigned long int i = 0; i < size; i++)
    {
      client.print(data[i]); // Send data
    }
    client.stop(); // Close the connection
  }
}

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
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  Ethernet.begin(mac); // Ethernet.begin(mac,ip);
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
      // give the web browser time to receive the data
      delay(1);
      // close the connection:
      client.stop();
      Serial.println("client disconnected");
    }
  }
}