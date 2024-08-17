from http import server
import socket
import struct
import threading
import time

from zmq import NULL
from MongoDB import MongoDB
import netifaces
from colored_terminal import *
#convertion to exe: pyinstaller --onefile --distpath ./compiled .\src\Serverv8.py

# troubleshooting tools:
# "ipconfig"
# "netstat" and then  "netstat -an | findstr "192.168.178.23:8888""

IP_SERVER = ("169.254.218.4") # if of Ethernet-Adapter Ethernet 6 # ip_Desktop = '192.168.178.23'
PORT = 8888

PACKET_LENTH = 472

TIMEOUT_CLIENT = 2.0 #s timeout of client after no connetion
DELAY_DATALOG_LOOP = 0.1 #s slows datalogger loop to save performance
DELAY_SERVER_LOOP = 0.01 #s slows server loop to save performance
DELAY_CONSOLE_LOOP = 0.1 #s slows console loop to save performance
DELAY_ERROR = 0.5 # delay to prevent errors to be spammed 

class TCP_SERVER:
    """Connects in an extra thread a TCP server.\
        Connects to avaliable clients and recieves inoomming data\
         can send data up"""
    def __init__(self, _ipadress=IP_SERVER, _port=PORT):
        self.datalog = DATALOGGER()
        self.ipadress = _ipadress
        self.port = _port
        self.__isRunning = 1
        self.command = {
            "command": '0',
            "param1": 0.0,
            "param2": 0.0,
            "param3": 0.0,
            "param4": 0.0,
        }
        self.thread = threading.Thread(target=self.StartServer)
        self.thread.start()

    def StartServer(self):
        """startet eine TCP Server an den sich ein client_socket verbinden kann"""

        def connect_server_socket(timeout=TIMEOUT_CLIENT):
            try:
                server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                server_socket.bind((self.ipadress, self.port))
                server_socket.settimeout(timeout)
                return server_socket
            
            except OSError as e:
                if e.errno == 10048:print_cyan("Address already in use.\n", indent=0)
                elif e.errno == 10049:print_cyan("Probably cable not connected.\n", indent=0)
                elif e.errno == 10038:print_cyan("Ein Vorgang bezog sich auf ein Objekt, das kein Socket ist. Probably no Ethernet dongle\n", indent=0)
                else:print_red(f"Error:{e} ", indent=2)

                time.sleep(DELAY_ERROR)
                return None
                
            except Exception as e:  # Handle other exceptions
                print_red(f"Error Connect Server:{e}\n", indent=2)
                time.sleep(DELAY_ERROR)
                return None

        def connect_client(server_socket: socket.socket, timeout=TIMEOUT_CLIENT, counter=3):
            """wartet auf einen client_socket und verbindet sich  mit demselben"""
            try:
                server_socket.listen(1)  # Wartet auf 1 eingehende Verbindung
                client_socket, client_address = server_socket.accept()
                clear_console_line()
                print_cyan(f"Verbindung hergestellt von:{client_address}\n", indent=1)
                client_socket.settimeout(timeout)
                return client_socket
            
            except socket.timeout:
                clear_console_line()
                print_cyan("waiting for a connection[" + counter * "."+"]")
                return None
            
            except OSError as e:
                if e.errno == 10038: print_cyan("Error Connect Client: Probably no Ethernet dongle\n", indent=0)
                else: print_red(f"Error Connect Client: {e} ", indent=2)

                time.sleep(DELAY_ERROR)
                server_socket.close()
                return None

            except Exception as e:  # Handle other exceptions
                print_red(f"Error Connect Client: {e} ", indent=2)
                time.sleep(DELAY_ERROR)
                server_socket.close()
                return None

        def recieve_data(client_socket):
            """recieves data in binary form from the client_socket and appends it to the rawdata of the datalogger"""
            success = 1
            try:
                ##recieves any data
                received_data = client_socket.recv(PACKET_LENTH)
                if received_data is not None:
                    #this is a packat being downlinked
                    if(len(received_data)==PACKET_LENTH):  
                        self.datalog.rawdata.append(received_data)
                        print_green(f'recieved packet ID:"{ struct.unpack('<I', received_data[:4])[0]}"\n',indent =2)
                    #this is an error code being downlinked
                    elif(len(received_data) == 8 and received_data[0]==0b11111111 and received_data[1]==0b11111111 and received_data[2]==0b11111111 and received_data[3]==0b11111111): 
                            if(received_data[4] == received_data[5]): 
                                print_red(f'recieved error code: "{received_data[4]}"\n',indent =2)
                            else:
                                print_red(f'recieved error codes corrupted. recieved: "{received_data[4]}" and "{received_data[5]}"\n',indent =2)
                    #this is a debug message being downlinked.
                    else: 
                            received_data = received_data.decode("utf-8",errors='ignore')
                            print_yellow(f'debug: "{received_data}"\n',indent =2)

                else:
                    print_red("duno wtf this is\n", indent=2)
                    print_cyan(f"Length of Recieved Data: {len(received_data)}\n")
                    print_cyan(f"data where error occured {received_data}\n")
                    success = 0

            except socket.timeout:
                # print_cyan("Datastream stopped. Socket timed out\n", indent=2)
                success = 0

            except ConnectionResetError:  # Handle connection reset by peer
                print_red("\nConnectionResetError aka Stecker gezogen / uC Reset\n", indent=2)
                client_socket.close()
                success = 0

            except OSError as e:     
                if e.errno == 10038:print_cyan("WinError 10038: Ein Vorgang bezog sich auf ein Objekt, das kein Socket ist. Probably no Ethernet dongle\n", indent=0)
                else:print_red(f"Error Recieve Data:{e} ", indent=2)
                client_socket.close()
                time.sleep(DELAY_ERROR)
                success = 0
                
            except Exception as e:  # Handle other exceptions
                print_red(f"Error Recieve Data:{e}\n", indent=2)
                client_socket.close()
                time.sleep(DELAY_ERROR)
                success = 0

            return success

        def send_command(client_socket: socket.socket):
            """Sends a command to the client"""
            try:
                # sends command
                print_cyan(f"Sending Command:{self.command["command"]} {self.command["param1"]} {self.command["param2"]} {self.command["param3"]} {self.command["param4"]}\n")
                comand = self.command["command"]
                param1 = float(self.command["param1"])
                param2 = float(self.command["param2"])
                param3 = float(self.command["param3"])
                param4 = float(self.command["param4"])
                uploadbuffer = comand.encode("utf-8")*3 + struct.pack("ffffffff", param1,param1,param2,param2,param3,param3,param4,param4)
                print(f'sending command: {comand}|{param1}|{param2}|{param3}|{param4}|\nin bytes: {uploadbuffer}')
                client_socket.sendall(uploadbuffer)
                # deletes command from buffer of server
                self.command = {"command": '0', "param1": 0.0, "param2": 0.0, "param3": 0.0, "param4": 0.0}
            except Exception as e:
                print_red(f"Error:{e}\n", indent=2)
                client_socket.close()

        server_socket = None
        client_socket = None
        counter_points = 0 #for thoes "waiting for client[...]" points

        print_cyan(f'<<<<starting TCP server>>>>\n')
        
        print_white(f'ip_adress: {self.ipadress}\nport: {self.port}"\ntime: {time.asctime(time.localtime())}\n\n')

        while self.__isRunning:
            ##connecting Socket if none is already connected
            if not server_socket:
                print_blue("Connnecting Socket...\n", indent=1)
                server_socket = connect_server_socket()
                if not server_socket:
                    time.sleep(DELAY_ERROR)
            else:
                ##connecting client if none is already connected
                if not client_socket:
                    client_socket = connect_client(
                        server_socket, counter=counter_points
                    )
                    counter_points += 1
                    if counter_points > 3: counter_points = 0
                else:
                    print(client_socket)
                    ##[up link] sending command to client if there is one in the self.command buffer 
                    if self.command["command"] != '0':
                        send_command(client_socket)

                    ##[down link] recieves data if theres one avaliable. Disconnects client if theres any issues besides a timeout
                    while recieve_data(client_socket) and self.__isRunning:            
                        if self.command["command"] != '0': #here the possibility to send a command in bewtween downlinks
                            send_command(client_socket)

                    # client_socket.close()  # geht ohne??
                    # client_socket = None

            time.sleep(DELAY_SERVER_LOOP) #delay to conserve performance

        # handling for the closing of the thread
        if client_socket:
            client_socket.close()
        if server_socket:
            server_socket.close()

    def shutdown(self):
        print_cyan("<shutdown of server>\n")
        self.__isRunning = False
        self.thread.join()
        print_cyan("<Server down>\n", indent=1)
        self.datalog.shutdown()

class DATALOGGER:
    """everytime y write data in rawdata[] it is stored in a .csv and a mongoDB Server,\
        then deleted. Runs in a extra thread"""
    def __init__(self) -> None:
        self.__isRunning = 1
        self.filename = f'./rawdata[{"-".join(time.asctime().split()[1:4]).replace(":","-")}].bin' #"./rawdata.bin"
        self.rawdata = []
        self.mongodb = MongoDB("localhost:27017").connect()
        self.thread = threading.Thread(target=self.SavingLoop)
        self.thread.start()

    def SavingLoop(self):
        print_yellow(f'Saving bin to : {self.filename}\n')
        while self.__isRunning:
            if len(self.rawdata)>1:
                self.save_raw_mongo()
                if self.save_raw_bin():
                    print(f"saved {len(self.rawdata)} packages")
                    self.rawdata = []
            else:
                time.sleep(DELAY_DATALOG_LOOP) #delay to conserve performance

    def save_raw_mongo(self):
        sucess = 1
        for packet in self.rawdata:
            try:
                # self.mongodb.write_mongodb({packet[0]: packet[1]}, "Probe", "Probecollect")
                pass
            except Exception as e:
                print(e)
                break
        else:
            sucess = 0
        return sucess

    def save_raw_bin(self):
        """reads the rawdata from the buffer of the TCP_Server, reads them in rawdata.csv and deletes the rawdata in TCP_Server if saving was successful"""
        sucess = 1
        for el in self.rawdata:
            try:
                with open(self.filename, "ab") as f:
                    f.write(el)
                    f.close
            except FileNotFoundError:
                print("File not found.")
                sucess = 0
                break
            except IOError:
                print(IOError)
                print("Error reading the file.")
                sucess = 0
                break
            except Exception as e:
                print(e)
                sucess = 0
                break
        return sucess

    def shutdown(self):
        print_magenta("<shutting down Datalogger>\n")
        self.__isRunning = 0
        self.save_raw_bin()
        self.thread.join()
        print_magenta("<Datalogger down>\n", indent=1)

class INTERFACE:
    """for a terminal based controll over the server"""
    def __init__(self):
        self.isRunning = 1
        self.server = None
        self.loop()

    def loop(self):
        print_yellow("\n<<Interface TCP Server>>\n/? for help\n")
        while self.isRunning:
            time.sleep(DELAY_CONSOLE_LOOP) #delay to conserve performance
            try:
                self.Command(input())
            except Exception as e:
                print(e)
                break

    def server_shutdown(self):
        if self.server != None:
            self.server.shutdown()
            del self.server
            self.server = None
        else:
            print_white("No Server to shutdown\n")

    def Command(self, command: str):
        if len(command) > 1 and command[0] == "/":
            match command[1:2]:
                case "?":
                    print_yellow("<Help>")
                    print_yellow(
                        """
/r [optional: server ip adress] start server
/s shutdown server
/i get network info
/t get threads info
/q /quit quit this file
/c for sending a command to client
"""
                    )

                case "r":
                    if self.server is None:
                        param_ip_adress = command[3:].split()
                        print(param_ip_adress)
                        print(len(param_ip_adress))

                        if len(param_ip_adress)==0: 
                            self.server = TCP_SERVER(_ipadress=IP_SERVER)
                        elif len(param_ip_adress)==1:
                            self.server = TCP_SERVER(_ipadress=str(param_ip_adress[0]))
                        elif len(param_ip_adress)==2:
                            self.server = TCP_SERVER(_ipadress=str(param_ip_adress[0]),_port=int(param_ip_adress[1]))
                        else:
                            print("sometgin broke as me")

                    else:
                        print_yellow("Server already running\n")


                case "s":
                    self.server_shutdown()

                case "i":
                    """returns lovely networking funfacts for the whole family"""
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
                                print("  IP Address:", addr_info["addr"])
                                print("  Netmask:", addr_info["netmask"])
                                print("  Broadcast Address:", addr_info.get("broadcast"))
                        else:
                            print("  No IPv4 address")

                case "quit" | "q":
                    print_yellow("<shuting down Interface>\n")
                    self.server_shutdown()
                    self.isRunning = 0
                    print_yellow("<all down>\n", indent=1)
                case "t":
                    print_yellow("Running threads: ")
                    for thread in threading.enumerate():
                        print(thread.name)
                case "c":
                    if self.server is None:
                        print_red("No Server connected\n")
                    else:
                        print_yellow("Setting command for next uplink: ")
                        try:
                            command_uplink = (command[2:] + " 0 "*6).split()[:6] #filling missed spaces with 0
                            command_buff = {
                                "command": command_uplink[0],
                                "param1": float(command_uplink[1]),
                                "param2": float(command_uplink[2]),
                                "param3": float(command_uplink[3]),
                                "param4": float(command_uplink[4]),
                            }
                            print(command_buff)
                            self.server.command = command_buff
                        except Exception as e:
                            print(e)

                case _:
                    print_red(f"Unknown Command {command[1:]}\n")

if __name__ == "__main__":
    interface = INTERFACE()


