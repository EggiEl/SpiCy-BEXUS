import socket
import struct
import threading
import time
from MongoDB import MongoDB
import netifaces
from colored_terminal import *

# troubleshooting tools:
# "ipconfig"
# "netstat" and then  "netstat -an | findstr "192.168.178.23:8888""

IP_SERVER = ("169.254.218.4") # if of Ethernet-Adapter Ethernet 6 # ip_Desktop = '192.168.178.23'
PORT = 8888

TIMEOUT_SERVER = 1

def sleep_till_stop(stop_flag,time_s:float):
    """sleeps till time runs out or stop_flag is 0"""
    while time_s > 0 and stop_flag==1:
        time.sleep(0.01)  # Adjust the sleep interval as needed
        time_s -= 0.01

def get_network_info():
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
            "param1": 0,
            "param2": 0,
            "param3": 0,
            "param4": 0,
        }
        self.thread = threading.Thread(target=self.StartServer)
        self.thread.start()

    def StartServer(self):
        """startet eine TCP Server an den sich ein cSERVERlient_socket verbinden kann"""

        def connect_server_socket(timeout=TIMEOUT_SERVER):
            try:
                server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                server_socket.bind((self.ipadress, self.port))
                server_socket.settimeout(timeout)
                return server_socket
            except OSError as e:
                print_red(f"Error:{e} ", indent=2)
                if e.errno == 10048:  # WinError 10048: Address already in use
                    print_cyan("Address already in use.\n", indent=0)
                    return 0
                elif e.errno == 10049:
                    print_cyan("Probably cable not connected.\n", indent=0)
                    return 0
            except Exception as e:  # Handle other exceptions
                print_red(f"Error:{e}\n", indent=2)
                return 0

        def connect_client(server_socket: socket.socket, timeout=TIMEOUT_SERVER, counter=3):
            """wartet auf einen client_socket und verbindet sich  mit demselben"""
            try:
                server_socket.listen(1)  # Wartet auf 1 eingehende Verbindung
            except Exception as e:
                print_red(f"Error:{e}\n", indent=2)
                return 0
            try:
                client_socket, client_address = server_socket.accept()
                clear_console_line()
                print_cyan(f"Verbindung hergestellt von:{client_address}\n", indent=1)
                client_socket.settimeout(timeout)
                return client_socket
            except socket.timeout:
                clear_console_line()
                print_cyan("waiting for a connection[" + counter * "."+"]")
                return 0
            except Exception as e:  # Handle other exceptions
                print_red("Error:", e)
                while True:
                    pass
                return 0

        def recieve_data(client_socket: socket.socket):
            """recieves data in binary form from the client_socket"""
            success = 1
            try:
                timestamp = time.asctime(time.localtime())[::-1][::-1]
                received_data = client_socket.recv(200)
                self.datalog.rawdata.append((timestamp, received_data))
                if received_data:
                    print_green(f"packet len: {len(received_data)} bytes\n", indent=3)

                    if(len(received_data) == 8): #is a error code is downlinked
                        print_red(f'error code: {received_data[4]}\n')
                    
                else:
                    print_red("duno wtf this is\n", indent=2)
                    print_cyan(f"Length of Recieved Data: {len(received_data)}\n")
                    print_cyan(f"data where error occured {received_data}\n")
                    success = 0

            except socket.timeout:
                print_cyan("Datastream stopped. Socket timed out\n", indent=2)
            except ConnectionResetError:  # Handle connection reset by peer
                print_red(
                    "\nConnectionResetError aka Stecker gezogen / uC Reset\n", indent=2
                )
            except Exception as e:  # Handle other exceptions
                print_red(f"Error:{e}\n", indent=2)
            return success

        def senddata(client_socket: socket.socket):
            try:
                # sends command
                print_cyan(f"Sending Command:{self.command["command"]} {self.command["param1"]} {self.command["param2"]} {self.command["param3"]} {self.command["param4"]}\n")
                comand = self.command["command"]
                param1 = self.command["param1"]
                param2 = self.command["param2"]
                param3 = self.command["param3"]
                param4 = self.command["param4"]
                uploadbuffer = comand.encode("utf-8")*2 + struct.pack("ffffffff", param1,param1,param2,param2,param3,param3,param4,param4)
                print(f'sending command: {uploadbuffer}')
                client_socket.sendall(uploadbuffer)
                # deletes command from buffer of server
                for k in self.command.keys():
                    self.command[k] = 0
                self.command["command"] = '0'
            except Exception as e:
                print_red(f"Error:{e}\n", indent=2)

        server_socket = 0
        client_socket = 0
        counter_recieved = 0 #to keep track how mutch packets were recieved
        counter_points = 0 #for thoes "waiting for client[...]" points

        print_cyan(
            f'<starting TCP server at "{self.ipadress}" | \
            "{time.asctime(time.localtime())}>"\n'
        )

        while self.__isRunning:
            #connecting Server
            if not server_socket:
                print_blue("Connnecting Socket...\n", indent=1)
                server_socket = connect_server_socket()
                if server_socket == 0:
                    sleep_till_stop(stop_flag=self.__isRunning,time_s=3)
            else:
                #connecting Client
                if not client_socket:
                    client_socket = connect_client(
                        server_socket, counter=counter_points
                    )
                    counter_points += 1
                    if counter_points > 3:
                        counter_points = 0
                else:
                    #up link
                    if self.command["command"] != '0':
                        senddata(client_socket)
                    
                    status = 1
                    while status and self.__isRunning:            
                        if self.command["command"] != '0': #up link
                            senddata(client_socket)

                        status = recieve_data(client_socket) #down link
                        counter_recieved += 1

                    client_socket.close()  # geht ohne??
                    client_socket = 0 

            time.sleep(0.01) #delay to conserve performance

        # handling for the closing of the thread
        if client_socket:
            client_socket.close()
        if server_socket:
            server_socket.close()

    def shutdown(self):
        print_cyan("<shutdown of server>\n")
        self.__isRunning = 0
        self.thread.join()
        print_cyan("<Server down>\n", indent=1)
        self.datalog.shutdown()

class DATALOGGER:
    """everytime y write data in rawdata[] it is stored in a .csv and a mongoDB Server,\
        then deleted. Runs in a extra thread"""
    def __init__(self) -> None:
        self.__isRunning = 1
        self.rawdata = []
        self.mongodb = MongoDB("localhost:27017").connect()
        self.thread = threading.Thread(target=self.SavingLoop)
        self.thread.start()

    def SavingLoop(self):
        while self.__isRunning:
            if len(self.rawdata)>1:
                self.save_raw_mongo()
                if self.save_raw_csv():
                    print(f"saved {len(self.rawdata)} packages")
                    self.rawdata = []
            else:
                # sleep_till_stop(1,self.__isRunning)
                time.sleep(0.01) #delay to conserve performance

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

    def save_raw_csv(self):
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

    def shutdown(self):
        print_magenta("<shutting down Datalogger>\n")
        self.__isRunning = 0
        self.save_raw_csv()
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
            time.sleep(0.01) #delay to conserve performance
            try:
                self.Command(input(".\n"))
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
/r /start start server
/s /stop shutdown server
/i /info get network info
/t /threads get threads info
/q /quit quit this file
/c for sending a command
"""
                    )

                case "start" | "run" | "r":
                    if self.server is None:
                        self.server = TCP_SERVER()
                    else:
                        print_yellow("Server already running\n")

                case "stop" | "s":
                    self.server_shutdown()

                case "info" | "i":
                    get_network_info()

                case "quit" | "q":
                    print_yellow("<shuting down Interface>\n")
                    self.server_shutdown()
                    self.isRunning = 0
                    print_yellow("<all down>\n", indent=1)
                case "threads" | "t":
                    print_yellow("Running threads:")
                    for thread in threading.enumerate():
                        print(thread.name)
                case "c":
                    if self.server is None:
                        print_red("No Server connected\n")
                    else:
                        print_yellow("Sending Command: ")
                        try:
                            command_uplink = (command[2:] + " 0 "*6).split()[:6] #filling missed spaces with 0
                            print(command_uplink)
                            command_buff = {
                                "command": command_uplink[0],
                                "param1": float(command_uplink[1]),
                                "param2": float(command_uplink[2]),
                                "param3": float(command_uplink[3]),
                                "param4": float(command_uplink[4]),
                            }
                            self.server.command = command_buff
                        except Exception as e:
                            print(e)

                case _:
                    print_red(f"Unknown Command {command[1:]}\n")

if __name__ == "__main__":
    interface = INTERFACE()
