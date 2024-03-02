#define MISO_LAN 16
#define CS_LAN 17
#define SCK_LAN 18
#define MOSI_LAN 19
#define SPI_FREQ_LAN 12MHz  //no need to change

#define MAC { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
#define SERVERIP IPAddress(169, 254, 171, 44)  //(192, 168, 178, 23)  // IP address of the PC
// #define SERVERIP IPAddress(100, 81, 57, 236)
#define CLIENTIP IPAddress(169, 254, 1, 2)  //ip of this uC, leve blank for auto generation IPAddress(169, 168, 1, 100)
// #define DNS IPAddress(8, 8, 8, 8)             // DNS server (e.g., Google DNS)
// #define GATEWAY IPAddress(192, 168, 1, 1)
#define SUBNET IPAddress(255, 255, 0, 0)

#define SERVERPORT 8888

#define CONNECTIONTIMEOUT 10000

volatile uint8_t TCP_init = 0;

EthernetClient client;

struct test {
  int a = 5;
  float b = 4.4;
} testa;

void test_TCP() {
  //packet send
  struct packet data;
  for (;;) {
    data = packet_create();
    data.oxigen[2] = 200;
    data.oxigen[1] = 100;
    packet_writeinfo(data, "Dies ist der Test der des structs Packet");
    // packet_print(data);
    data.timestampPacket = millis();
    while (send_TCP_packet(data) != 1) {
    };
    // delay(500);
  }

  //str recieve
  // for (unsigned int i = 0; i < 100000; i++) {
  //   char* buffer = recieve_TCP();
  //   if (buffer) {
  //     int a = 0;
  //     while (buffer[a] != '\n') {
  //       Serial.print(buffer[a]);
  //       a++;
  //     }
  //     Serial.println();
  //   }
  //   delay(1000);
  // }
}

//sets the RP2040 as TCP Client for the given ip adress
void setup_TCP_Client() {
  debugln("-{setup_TCP_Client-");
  MESSURETIME_START

  byte mac[] = MAC;  // MAC address
  SPI.setRX(MISO_LAN);
  SPI.setTX(MOSI_LAN);
  SPI.setSCK(SCK_LAN);

  Ethernet.setRetransmissionCount(8);
  Ethernet.setRetransmissionTimeout(200);  //miliseconds
  client.setConnectionTimeout(CONNECTIONTIMEOUT);

  Ethernet.init(CS_LAN);

  //----automatic ip allocation : ----
  // if (!Ethernet.begin(mac)) {
  //   debug("DHCP configuration failed}-");
  //   debugln("-fail}-");
  //   return;
  // }

  //-----manual (static) ip--------------

  Ethernet.begin(mac, CLIENTIP);  //mac, CLIENTIP, DNS, GATEWAY, SUBNET
  Ethernet.setSubnetMask(SUBNET);
  // Ethernet.setLocalIP(CLIENTIP);


  //--------debugg prints--------------
  debug("-ip_Server: ");
  debugln(SERVERIP);

  debug("-mac: ");
  uint8_t* mac_address = (uint8_t*)malloc(8);
  Ethernet.MACAddress(mac_address);
  for (uint8_t i = 0; i < 6; i++) {
    if (DEBUG) { Serial.print(mac_address[i], HEX); }
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
  debugln(Ethernet.hardwareStatus());

  cabletest();

  // for (int i = 0; i < 1000; i++) { dosen´t work for some reason?!?!
  //   if (client) {
  //     TCP_init = 1;
  //     break;
  //   }
  // }

  while (!client) {}
  TCP_init = 1;

  MESSURETIME_STOP

  if (TCP_init) {
    debugln("-success}-");
  } else {
    debugln("-clientinittimeout-failed}-");
  }
}

//sends an dynamic char array as an TCP client
void send_TCP(char* data, unsigned long int size) {
  if (size == 0) {
    Serial.println("Error: Invalid size. Size must be greater than 0.");
    return;
  }
  if (client.connect(SERVERIP, 8888)) {
    for (unsigned long int i = 0; i < size; i++) {
      client.print(data[i]);  // Send data
    }
    client.stop();  // Close the connection
  }
}

char* recieve_TCP() {
  debug("-{recieve_TCPdTCPpacket-id:");
  if (!TCP_init) {
    setup_TCP_Client();
  }

  if (!client) {
    client.connect(SERVERIP, SERVERPORT);
  }

  int size = client.available();

  if (!size) {
    debugln("-noavaliable}-");
    return 0;
  }

  char* buffer = (char*)malloc(size + 1);
  for (int i = 0; i < size; i++) {
    signed char data = client.read();  //Returns The next byte (or character), or -1 if none is available.
    if (data == -1) {
      debug("-client.read()=-1-");
      break;
    }
    buffer[i] = data;
  }
  buffer[size + 1] = '\n';
  debugln("-success}-");
  return buffer;
}


//converts the packet in a char array and sends this as TCP Client to a Pc
int send_TCP_packet(struct packet data) {
  debug("-{sendTCPpacket-id:");
  debug(data.id);
  debug("-");
  MESSURETIME_START

  if (!TCP_init) { setup_TCP_Client(); }

  char* buffer = (char*)malloc(sizeof(struct packet));  //converts struct in a char array
  memcpy(buffer, &data, sizeof(struct packet));

  int status = 1;
  //needs work when sending is implemented
  // if (client.available()) { status = client.connect(SERVERIP, SERVERPORT); }

  if (!client.connected()) {  //Whether or not the client is connected. Note that a client is considered connected if the connection has been closed but there is still unread data.
    debug("-connect_client-");
    status = client.connect(SERVERIP, SERVERPORT);
  }

  switch (status) {  //client.connect returns different int values depending of the sucess of the operation
    case 1:
      client.write(buffer, sizeof(struct packet));  // Send data
      // client.flush();                               //waits till all is send //can be left out if TCP Server recives just 200bytes per package.unsave?
      MESSURETIME_STOP
      debugln("-success}-");
      return 1;
    case -1:
      cabletest();
      debugln("-TIMED_OUT}-");
      return -1;
    case -2:
      cabletest();
      debugln("-INVALID_SERVER}-");
      return -2;
    case -3:
      cabletest();
      debugln("-TRUNCATED}-");
      return -3;
    case -4:
      cabletest();
      debugln("-TRUNCATED}-");
      return -4;
    default:
      debug("Case: ");
      debug(status);
      cabletest();
      debugln("-i think no response from server or hardware error}-");
      return -69;
  }
}

//Serial prints in debug mode weather cable is connected or not
void cabletest() {
  debug("-cable: ");
  switch (Ethernet.linkStatus()) {
    case 1:
      debug("con-");
      break;
    case 2:
      debug("discon_or_iC_faulty-");
      break;
    default:
      debug(Ethernet.linkStatus());
  }
}