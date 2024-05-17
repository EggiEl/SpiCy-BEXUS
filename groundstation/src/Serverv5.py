import socket
import threading
import time
from MongoDB import MongoDB
import netifaces
from colored_terminal import *

# troubleshooting tools:
# "ipconfig"
# "netstat" and then  "netstat -an | findstr "192.168.178.23:8888""

IP_SERVER = (
    "169.254.218.4"  # if of Ethernet-Adapter Ethernet 6 # ip_Desktop = '192.168.178.23'
)
PORT = 8888


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
                print("  IP Address:", addr_info["addr"])
                print("  Netmask:", addr_info["netmask"])
                print("  Broadcast Address:", addr_info.get("broadcast"))
        else:
            print("  No IPv4 address")


class TCP_SERVER:

    def __init__(self, _ipadress=IP_SERVER, _port=PORT):
        self.datalog = DATALOGGER()
        self.ipadress = _ipadress
        self.port = _port
        self.__isRunning = 1
        self.thread = threading.Thread(target=self.StartServer)
        self.thread.start()

    def StartServer(self):
        """startet eine TCP Server an den sich ein myclient verbinden kann"""

        def connect_server_socket(timeout=2) -> socket:
            try:
                server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                server_socket.bind((self.ipadress, self.port))
                server_socket.settimeout(timeout)
                return server_socket
            except OSError as e:
                print_red(f"Error:{e}\n",indent=2)
                if e.errno == 10048:  # WinError 10048: Address already in use
                    print_cyan("Address already in use.\n",indent=2)
                    return 0
                elif e.errno == 10049:
                    print_cyan("Probably cable not connected.\n",indent=2)
                    return 0
            except Exception as e:  # Handle other exceptions
                print_red(f"Error:{e}\n",indent=2)
                return 0

        def connect_client(server_socket: socket, timeout=2):
            """wartet auf einen myclient und verbindet sich  mit demselben"""
            try:
                server_socket.listen(1)  # Wartet auf 1 eingehende Verbindung
            except Exception as e:
                print_red(f"Error:{e}\n",indent=2)
                return 0

            counter = 0
            try:
                myclient, client_address = server_socket.accept()
                clear_console_line()
                print_cyan(f" | Verbindung hergestellt von:{client_address}\n")
                myclient.settimeout(timeout)
                return myclient
            except socket.timeout:
                match counter:  # aestetics
                    case 0:
                        clear_console_line()
                        print_cyan("waiting for a connection")
                        counter += 1
                    case 1:
                        clear_console_line()
                        print_cyan("waiting for a connection.")
                        counter += 1
                    case 2:
                        clear_console_line()
                        print_cyan("waiting for a connection..")
                        counter += 1
                    case 3:
                        clear_console_line()
                        print_cyan("waiting for a connection...")
                        counter = 0
                    case _:
                        pass
                return 0
            except Exception as e:  # Handle other exceptions
                print_red("Error:", e)
                return 0

        def recieve_data(myclient):
            """recieves data in binary form from the myclient"""
            success = 0
            try:
                timestamp = time.asctime(time.localtime())[::-1][::-1]
                received_data = myclient.recv(200)
                self.datalog.rawdata.append((timestamp, received_data))
                if received_data:
                    success = 1
                else:
                    print_red("duno wtf this is",indent=2)
                    print_cyan(f"Length of Recieved Data: {len(received_data)}\n")
                    print_cyan(f"data where error occured {received_data}\n")

            except socket.timeout:
                print_cyan("Datastream stopped. Socket timed out\n",indent=2)
            except ConnectionResetError:  # Handle connection reset by peer
                print_red("\nConnectionResetError aka Stecker gezogen / uC Reset\n",indent=2)
            except Exception as e:  # Handle other exceptions
                print_red(f"Error:{e}\n",indent=2)
            return success

        print_cyan(
            f'<starting TCP server at "{self.ipadress}" | "{time.asctime(time.localtime())}>"\n'
        )
        while self.__isRunning:
            server_socket = 0
            myclient = 0
            counter_recieved = 0
            if not server_socket:
                print_white("Connnecting Socket...\n",indent=1)
                server_socket = connect_server_socket()
            else:
                if not myclient:
                    print_white("Connnecting Client...\n",indent=1)
                    myclient = connect_client(server_socket)
                else:
                    status = 1
                    while status:
                        status = recieve_data(myclient)
                        counter_recieved += 1
                    myclient.close()  # TESTING

        # handling for the closing of the thread
        if myclient:
            myclient.close()
        if server_socket:
            server_socket.close()

    def shutdown(self):
        print_cyan("<shutdown of server>\n")
        self.__isRunning = 0
        self.thread.join()
        print_cyan("<Server down>\n",indent=1)
        self.datalog.shutdown()


class DATALOGGER:
    """datalogger takes data from listener and saves it in corresponding files"""

    def __init__(self) -> None:
        self.__isRunning = 1
        self.rawdata = []
        self.mongodb = MongoDB("localhost:27017").connect()
        self.thread = threading.Thread(target=self.SavingLoop)
        self.thread.start()

    def SavingLoop(self):
        while self.__isRunning:
            self.save_raw_mongo()
            if self.save_raw_csv():
                print(f"saved {len(self.rawdata)} packages")
                self.rawdata = []

    def save_raw_mongo(self):
        sucess = 1
        for packet in self.rawdata:
            try:
                self.mongodb.write_mongodb(
                    {packet[0]: packet[1]}, "Probe", "Probecollect"
                )
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
        self.thread.join()
        print_magenta("<Datalogger down>\n", indent=1)


class INTERFACE:
    def __init__(self):
        self.isRunning = 1
        self.server = None
        self.loop()

    def loop(self):
        print_yellow("\n<<Interface TCP Server>>\n/? for help\n")
        while self.isRunning:
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
            match command[1:]:
                case "?":
                    print_yellow("<Help>")
                    print_yellow(
                        """
/start start server
/stop shutdown server
/info get network info
/quit quit this file
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
                    print_yellow("<all down>\n,indent=1")
                case _:
                    print_red(f"Unknown Command {command[1:]}\n")


if __name__ == "__main__":
    interface = INTERFACE()
