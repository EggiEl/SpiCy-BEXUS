import socket
import struct
import threading
import time

# ip_uC = '192.168.178.23'
ip_Laptop = '169.254.171.44'
ip_Desktop = '192.168.178.23'
port = 8888

def millis():
    return round(time.time() * 1000)

def get_ip_address():
    """returns some randomas Ip adress. dkn wich one. dk"""
    try:
        # Create a socket object
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        # Connect to a remote server (doesn't matter which one)
        s.connect(("8.8.8.8", 80))
        # Get the socket's own IP address
        ip_address = s.getsockname()[0]
        # Close the socket
        s.close()
        return ip_address
    except Exception as e:
        print("Error:", e)
        return None
    
class TCP_SERVER:
    def __init__(self, _struct_format="L L 6L 6f 6f 6i i f 2i 80s"):
        self.packets = []
        self.rawdata = []
        self.ipadress= ip_Laptop
        self.port = 8888
        self.isRunning = 1
        self.struct_format = _struct_format
        self.thread = threading.Thread(target=self.StartServer)
        self.thread.start()

    def StartServer(self):  
        """startet eine TCP Server an den sich ein Client verbinden kann"""          
        counter_recieved = counter_corrupted = 0
        client_socket = client_address = 0
        print(f'- starting TCP server at "{self.ipadress}" at "{time.asctime(time.localtime())}"')
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Erstellen Sie einen Socket für den Server
        
        while True:
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
            
        client_socket, client_address = ConnectClient()
        timestamp_server = millis()-1 #-1 damit später be der packet/s berechnung kein 0 unterm bruch landen kann
            
        while self.isRunning:
            
            #recieve binary
            try:
                timestamp= time.asctime(time.localtime())[::-1][::-1]
                received_data = client_socket.recv(200)
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
            
            #refreshed output ab 1000 recieved packages nur noch alle 100 recieved packages
            if counter_recieved<1000 or counter_recieved % 100 == 0: 
                print("\r" + " " * 70 + "\r", end='', flush=True)  # Clear the  last line
                print(f'- recieved_structs: {counter_recieved} | sizeof(latest_data): {len(received_data)} bytes | corrupted: {counter_corrupted} | packets/sec: {round(1000*counter_recieved/(millis()-timestamp_server),2)}', end='',flush=True)


               #convert binary to struct
            try:
                self.rawdata.append((received_data,time.time()))
                unpacked_data = struct.unpack(self.struct_format, received_data) # Entpacken Sie die Daten in die Struktur
                self.packets.append((unpacked_data,timestamp))
            except:
                print("\nstruct_format is wrong or data is corrupted")
                print(f"Length of Recieved Data: {len(received_data)}")
                print(f"Recieved Data: {received_data}")
                print()
                counter_corrupted += 1


        server_socket.close()  # Schließen Sie den Server-Socket

class DATALOGGER:
    """datalogger takes data from listener and saves it in corresponding files"""
    def __init__(self) -> None:
        self.rawdata = "rawdata.csv"
        self.formateddata = "data.csv"
        self.header()

    def header(self):
        f = open(self.formateddata, "a")
        header = [
            "timestamp save",
            "id",
            "timestampPacket",
            [f'timestampOxy{i}' for i in range(6)],
            [f'oxigen{i}' for i in range(6)],
            [f'tempTube{i}' for i in range(6)],
            [f'heaterPWM{i}' for i in range(6)],
            "info",
            "tempCpu",
            "Voltage",
            "Current",
            "error",
        ]
        f.write("\n")
        for elem in header:
            if type(elem) is str:
                f.writelines(elem)
                f.write(";")
            if type(elem) is list:
                for i in elem:
                    f.writelines(i)
                    f.write(";")
        f.write("\n")
        f.close

    def write_packet(self, packet):
        try:
            with open(self.formateddata, "a") as f:
                f.write(packet[1])
                f.write(";")
                for elem in packet[0]:
                    f.writelines(str(elem))
                    f.write(";")
                f.write("\n")
                f.close
                return 1
        except FileNotFoundError:
            print("File not found.")
            return 0
        except IOError:
            print("Error reading the file.")
            return 0
    
    def savepackets(self,TCP_server:TCP_SERVER):
        """reads the packets from the buffer of the TCP_Server, reads them in data.csv and deletes the packets in TCP_Server if saving was successful"""
        packets = TCP_server.packets
        sucess = 1
        for el in packets:
            if not self.write_packet(el):
                sucess = 0
        if sucess:
            TCP_server.packets = []
        else:
            print("packet convertion to data.csv went wrong")
    
    def write_rawdata(self,rawdata):
        try:
            with open(self.rawdata, "a") as f:
                f.write(str(rawdata[1]))
                f.write(";")
                f.write(str(rawdata[0]))
                f.write(";")
                f.write("\n")
                f.close
                return 1
        except FileNotFoundError:
            print("File not found.")
            return 0
        except IOError:
            print("Error reading the file.")
            return 0
        
    def saveraw(self,TCP_server:TCP_SERVER):
        """reads the rawdata from the buffer of the TCP_Server, reads them in rawdata.csv and deletes the rawdata in TCP_Server if saving was successful"""
        sucess = 1
        for el in TCP_server.rawdata:
            if not self.write_rawdata(el):
                sucess = 0
        if sucess:
            TCP_server.rawdata = []
        else:
            print("rawdata convertion to rawdata.csv went wrong")

if __name__ == "__main__":

    # 	a = TCP_SERVER()
    # # 	# while True:
    # 	time.sleep(2)
    # 	print(a.packets[0])
    # 	a.isRunning = 0

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
 
    a = TCP_SERVER()
    datalog = DATALOGGER()
    time.sleep(2)
    
    while True:
        datalog.savepackets(a)
        datalog.saveraw(a)
        time.sleep(2)
