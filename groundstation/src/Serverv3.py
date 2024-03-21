import socket
import struct
import threading
import time
from MongoDB import MongoDB 
import netifaces

def get_network_info():
    # Get host name and IP address
    hostname = socket.gethostname()
    ip_address = socket.gethostbyname(hostname)

    print("Hostname:", hostname)
    print("IP Address:", ip_address)

    # Get information about network interfaces
    print("\nNetwork Interfaces:")
    interfaces = netifaces.interfaces()
    for iface in interfaces:
        iface_details = netifaces.ifaddresses(iface)
        print("Interface:", iface)
        if netifaces.AF_INET in iface_details:
            for addr_info in iface_details[netifaces.AF_INET]:
                print("  IP Address:", addr_info['addr'])
                print("  Netmask:", addr_info['netmask'])
                print("  Broadcast Address:", addr_info.get('broadcast'))
        else:
            print("  No IPv4 address")
# ip_uC = '192.168.178.23'

# troubleshooting tools:
#"ipconfig"
#"netstat" and then  "netstat -an | findstr "192.168.178.23:8888""
    
ip_Laptop = '169.254.218.4' #if of Ethernet-Adapter Ethernet 6
# ip_Laptop =" 169.254.218.4"
# ip_Laptop ='169, 254, 171, 44'
# ip_Laptop ='0,0,0,0'

# ip_Desktop = '192.168.178.23'
port = 8888

def millis():
    return round(time.time() * 1000)


class TCP_SERVER:
    def __init__(self):
        self.datalog = DATALOGGER()
        self.ipadress= ip_Laptop
        self.port = 8888
        self.__isRunning = 1
        self.thread = threading.Thread(target=self.StartServer)
        self.thread.start()

    def shutdown(self):
        print("")
        print("--shutdown of server--")
        self.__isRunning = 0
        self.datalog.isRunning = 0
        time.sleep(0.5)
        self.thread.join()
        self.datalog.thread.join()
        print("Server down")
        
    def StartServer(self):  
        """startet eine TCP Server an den sich ein Client verbinden kann"""          
        counter_recieved = counter_corrupted = 0
        client_socket = client_address = 0
        print(f'- starting TCP server at "{self.ipadress}" at "{time.asctime(time.localtime())}"')
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Erstellen Sie einen Socket für den Server
        # server_socket.settimeout(2)
        
        #tries to connect to sockel
        while self.__isRunning:
            try:
                server_socket.bind((self.ipadress, self.port))  # Binden Sie den Socket an den Host und Port
                break
            
            except OSError as e:
                print("Error:", e)
                if e.errno == 10048:  # WinError 10048: Address already in use
                    print("Address already in use. Retrying in 2 seconds...")
                    time.sleep(2)  # Wait for 1 seconds before retrying
                elif e.errno == 10049:
                    print("Probably cable not connected. Retrying in 2 seconds...")
                    time.sleep(2)  # Wait for 1 seconds before retrying
    
            except Exception as e: # Handle other exceptions  
                print("Error:", e)

        def ConnectClient():
            """wartet auf einen Client und verbindet auch mit demselben"""
            print('- Server wartet auf Verbindung',end='')
            # server_socket.settimeout(1)  # Set a timeout of 1 second
            server_socket.listen(1)  # Warten Sie auf eingehende Verbindungen
            client_socket, client_address = (server_socket.accept())  # Akzeptieren Sie eine Verbindung
            client_socket.settimeout(2)
            print(' | Verbindung hergestellt von:', client_address)
            return client_socket, client_address
        
        #first connection with Client
        client_socket, client_address = ConnectClient()
        timestamp_server = millis()-1 #-1 damit später be der packet/s berechnung kein 0 unterm bruch landen kann
            
        while self.__isRunning:
            
            #recieve binary
            try:
                timestamp= time.asctime(time.localtime())[::-1][::-1]
                received_data = client_socket.recv(200)
                self.datalog.rawdata.append((timestamp,received_data))
                if not received_data:
                    print("\n duno wtf this is")
                    print(f"Length of Recieved Data: {len(received_data)}")
                    print(f"data where error occured {received_data}")
                    client_socket, client_address = ConnectClient()
                    continue
                    
                
            except socket.timeout:
                print("")
                print("Socket timed out while waiting for struct")
                client_socket.close()  # Schließen Sie die Verbindung zum Client
                client_socket, client_address = ConnectClient()
                continue
            
            except ConnectionResetError:# Handle connection reset by peer
                print("\nConnectionResetError aka Stecker gezogen / uC Reset")
                client_socket.close()  # Schließen Sie die Verbindung zum Client
                client_socket, client_address = ConnectClient()
                continue

            except Exception as e: # Handle other exceptions  
                print("\nError:", e)
                client_socket.close()  # Schließen Sie die Verbindung zum Client
                client_socket, client_address = ConnectClient()
                continue
            
            counter_recieved += 1
            
            #refreshed terminal info ab 1000 recieved packages nur noch alle 100 recieved packages
            if counter_recieved<1000 or counter_recieved % 100 == 0: 
                print("\r" + " " * 70 + "\r", end='', flush=True)  # Clear the  last line
                print(f'- recieved_structs: {counter_recieved} | sizeof(latest_data): {len(received_data)} bytes | corrupted wip: {counter_corrupted} | packets/sec: {round(1000*counter_recieved/(millis()-timestamp_server),2)}', end='',flush=True)

        print("-closing socket")
        server_socket.close()  # schließt den Sockel wenn der Thread geschlossen wird

class DATALOGGER:
    """datalogger takes data from listener and saves it in corresponding files"""
    def __init__(self) -> None:
        self.rawdata = []
        self.mongodb = MongoDB("localhost:27017")
        self.mongodb.connect()
        self.isRunning = 1
        self.thread = threading.Thread(target=self.SavingLoop)
        self.thread.start()
    
    def SavingLoop(self):
        while(self.isRunning):
            self.saveraw_mongo()
            if(self.saveraw_csv()):
                print(f"saved {len(datalog.rawdata)} packages")
                self.rawdata = []
                
    
    def saveraw_mongo(self):
        sucess = 1
        for packet in self.rawdata:
            try:
                self.mongodb.write_mongodb({packet[0] : packet[1]},"Probe", "Probecollect")
            except Exception as e:
                print(e)
                break
        else:
            sucess = 0
        return sucess
        
    
    def saveraw_csv(self):
        """reads the rawdata from the buffer of the TCP_Server, reads them in rawdata.csv and deletes the rawdata in TCP_Server if saving was successful"""
        sucess = 1
        for el in self.rawdata:
            try:
                with open("./rawdata.csv", "a") as f:
                    f.write(str(el[0]))
                    f.write(";")
                    f.write(str(el[1]))
                    f.write(";")
                    f.write("\n")
                    f.close
            except FileNotFoundError:
                print("File not found.")
                break
            except IOError:
                print(IOError)
                print("Error reading the file.")
                break
            except Exception as e:
                print(e)
                break
        else:
            sucess = 0
        return sucess
        
        


if __name__ == "__main__":
    test_packet = (
        3522,
        0,
        0,
        0,
        0,
        0,
        0,
        0.0,
        100.0,
        200.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0.0,
        0,
        0,
        b"Hier steht die zu \xc3\xbcbertragende Info\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
    )
    # print(get_ip_address())
    
    server = TCP_SERVER()
    datalog = server.datalog
    # mongodb = MongoDB("localhost:27017" )
    # mongodb.connect()
    time.sleep(2)
    # get_network_info()    
    # server.shutdown()
    
    # for packet in datalog.rawdata:
    #     mongodb.write_mongodb({"test" : str(packet)}, "Sensor1", "Collection 1")
    
    # datalog.saveraw_csv()
