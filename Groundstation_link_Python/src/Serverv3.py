import socket
import struct
import threading
import time
import os
# from MongoDB import MongoDB 

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
        self.datalog = DATALOGGER()
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
        
        #tries to connect to sockel
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
        
        #first connection with Client
        client_socket, client_address = ConnectClient()
        timestamp_server = millis()-1 #-1 damit später be der packet/s berechnung kein 0 unterm bruch landen kann
            
        while self.isRunning:
            
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


        server_socket.close()  # schließt den Sockel wenn der Thread geschlossen wird

class DATALOGGER:
    """datalogger takes data from listener and saves it in corresponding files"""
    def __init__(self) -> None:
        self.rawdata = []

    def bintolist(binary_data,struct_format):
        """convert binary to (struct)list"""
        try:
            unpacked_data = struct.unpack(struct_format, binary_data) # Entpacken Sie die Daten in die Struktur
            return(unpacked_data)
        except:
            print("\nstruct_format is wrong or data is corrupted")
            print(f"Length of binary Data: {len(binary_data)}")
            print(f"Binary Data: {binary_data}")
            print()
        
    def saveraw_csv(self):
        """reads the rawdata from the buffer of the TCP_Server, reads them in rawdata.csv and deletes the rawdata in TCP_Server if saving was successful"""
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
                print("Error reading the file.")
                break
            else:
                pass #delete the packages from
        


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
 
    server = TCP_SERVER()
    datalog = server.datalog
    # mongodb = MongoDB("localhost:27017" )
    # mongodb.connect()
    time.sleep(5)    
    server.isRunning = 0
    
    # for packet in datalog.rawdata:
    #     mongodb.write_mongodb({"test" : str(packet)}, "Sensor1", "Collection 1")
    
    datalog.saveraw_csv()
