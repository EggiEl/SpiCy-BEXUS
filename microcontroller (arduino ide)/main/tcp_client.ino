#define MISO_LAN 16
#define CS_LAN 17
#define SCK_LAN 18
#define MOSI_LAN 19
#define SPI_FREQ_LAN 20000  //dosen´t work yet

// #define SERVERIP IPAddress(192, 168, 178, 23)  // IP address of the PC
#define SERVERIP IPAddress(192,168,178,32) //ip Laptop
#define SERVERPORT 8888
#define PORT 80

#define CONNECTIONTIMEOUT 1000

volatile uint8_t TCP_init = 0;

EthernetClient client;

struct test {
  int a = 5;
  float b = 4.4;
} testa;

void test_TCP() {
  //packet send
  struct packet data;
  for (unsigned int i = 0; i < 100000; i++) {
    data = packet_create();
    data.oxigen[2] = 200;
    data.oxigen[1] = 100;
    packet_writeinfo(data, "Dies ist der Test der des structs Packet");
    // packet_print(data);
    data.timestampPacket = millis();
    while (send_TCP_packet(data) != 1) {};
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
  debug("-{setup_TCP-");
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // MAC address
  // IPAddress ip(192, 168, 1, 100);                       // IP address of the RP2040, here automaticly generated
  SPI.setRX(MISO_LAN);
  SPI.setTX(MOSI_LAN);
  SPI.setSCK(SCK_LAN);

  Ethernet.setRetransmissionCount(8);
  Ethernet.setRetransmissionTimeout(200);  //miliseconds

  Ethernet.init(CS_LAN);
  if (!Ethernet.begin(mac)) {
    debug("DHCP configuration failed}-");
    debug("hardwareStatus:");
    debug(Ethernet.hardwareStatus());
    debugln("-fail}-");
    return;
  }

  debug("hardwareStatus:");
  debug(Ethernet.hardwareStatus());

  debug("local_IP::");
  debug(Ethernet.localIP());

  // for (int i = 0; i < 1000; i++) { dosen´t work for some reason?!?!
  //   if (client) {
  //     TCP_init = 1;
  //     break;
  //   }
  // }

  while (!client) {}
  TCP_init = 1;

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
  // unsigned long  ta = millis();
  // unsigned long  tb;
  MESSURETIME_START
  if (!TCP_init) {
    setup_TCP_Client();
  }

  char* buffer = (char*)malloc(sizeof(struct packet));
  memcpy(buffer, &data, sizeof(struct packet));  //converts struct in a char array

  client.setConnectionTimeout(CONNECTIONTIMEOUT);

  int status = 1;

  if (client.available()) { status = client.connect(SERVERIP, 8888); }

  if (!client.connected()) {  //Whether or not the client is connected. Note that a client is considered connected if the connection has been closed but there is still unread data.
    debug("-connect_client-");
    status = client.connect(SERVERIP, 8888);
  }


  switch (status) {  //client.connect returns different int values depending of the sucess of the operation
    case 1:
      client.write(buffer, sizeof(struct packet));  // Send data
      client.flush();                               //waits till all is send //test if necessary?
      // client.stop();
      MESSURETIME_STOP
      debugln("-success}-");
      return 1;
    case -1:
      debugln("-TIMED_OUT}-");
      return -1;
    case -2:
      debugln("-INVALID_SERVER}-");
      return -2;
    case -3:
      debugln("-TRUNCATED}-");
      return -3;
    case -4:
      debugln("-TRUNCATED}-");
      return -4;
    default:
      debug("Case: ");
      debug(status);
      debugln("-i think no response from server or hardware error}-");
      return -69;
  }
}
