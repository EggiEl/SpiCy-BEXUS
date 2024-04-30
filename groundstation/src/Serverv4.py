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


def millis():
    return round(time.time() * 1000)


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

    def shutdown(self):
        print_cyan("<shutdown of server>\n")
        self.__isRunning = 0
        self.datalog.shutdown()
        time.sleep(0.5)
        self.thread.join()
        self.datalog.thread.join()
        print_cyan("<Server down>\n")

    def StartServer(self):
        """startet eine TCP Server an den sich ein Client verbinden kann"""
        counter_recieved = 0
        counter_corrupted = 0
        client_socket = 0

        print_cyan(
            f'<starting TCP server at "{self.ipadress}" | "{time.asctime(time.localtime())}>"\n'
        )

        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.settimeout(2)

        # tries to connect to sockel
        while self.__isRunning:
            try:
                server_socket.bind((self.ipadress, self.port))
                break

            except OSError as e:
                print_red("Error:", e)
                if e.errno == 10048:  # WinError 10048: Address already in use
                    print_cyan("Address already in use. Retrying in 2 seconds...")
                    time.sleep(2)  # Wait for 1 seconds before retrying
                elif e.errno == 10049:
                    print_cyan("Probably cable not connected. Retrying in 2 seconds...")
                    time.sleep(2)  # Wait for 1 seconds before retrying

            except Exception as e:  # Handle other exceptions
                print_red("Error:", e)

        def ConnectClient():
            """wartet auf einen Client und verbindet sich  mit demselben"""

            server_socket.listen(1)  # Warten Sie auf eine eingehende Verbindung

            counter = 0
            while self.__isRunning:
                try:
                    client_socket, client_address = server_socket.accept()
                    client_socket.settimeout(2)
                    clear_console_line()
                    print_cyan( f" | Verbindung hergestellt von:{client_address}\n")
                    return client_socket
                except socket.timeout:
                    match counter:
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
                except Exception as e:  # Handle other exceptions
                    print_red("Error:", e)

        # first connection with Client
        client_socket = ConnectClient()
        timestamp_server = (
            millis() - 1
        )  # -1 damit später be der packet/s berechnung kein 0 unterm bruch landen kann

        while self.__isRunning:

            # recieve binary
            try:
                timestamp = time.asctime(time.localtime())[::-1][::-1]
                received_data = client_socket.recv(200)
                self.datalog.rawdata.append((timestamp, received_data))
                if not received_data:
                    print_red("\n duno wtf this is")
                    print_cyan(f"Length of Recieved Data: {len(received_data)}\n")
                    print_cyan(f"data where error occured {received_data}\n")
                    client_socket = ConnectClient()
                    continue

            except socket.timeout:
                print_cyan("Datastream stopped. Socket timed out\n")
                client_socket.close()  # Schließen Sie die Verbindung zum Client
                client_socket = ConnectClient()
                continue

            except ConnectionResetError:  # Handle connection reset by peer
                print_red("\nConnectionResetError aka Stecker gezogen / uC Reset\n")
                client_socket.close()  # Schließen Sie die Verbindung zum Client
                client_socket = ConnectClient()
                continue

            except Exception as e:  # Handle other exceptions
                print_red(f"\nError:{e}\n")
                client_socket.close()  # Schließen Sie die Verbindung zum Client
                client_socket = ConnectClient()
                continue

            counter_recieved += 1

            # refreshed terminal info ab 1000 recieved packages nur noch alle 100 recieved packages
            if counter_recieved < 1000 or counter_recieved % 100 == 0:
                clear_console_line()
                print_blue(f"- recieved_structs: {counter_recieved} | sizeof(latest_data): {len(received_data)} bytes | corrupted wip: {counter_corrupted} | packets/sec: {round(1000*counter_recieved/(millis()-timestamp_server),2)}\n")
                

        print_cyan("<closing socket>\n")
        server_socket.close()  # schließt den Sockel wenn der Thread geschlossen wird


class DATALOGGER:
    """datalogger takes data from listener and saves it in corresponding files"""

    def __init__(self) -> None:
        self.rawdata = []
        self.mongodb = MongoDB("localhost:27017")
        self.mongodb.connect()
        self.__isRunning = 1
        self.thread = threading.Thread(target=self.SavingLoop)
        self.thread.start()

    def SavingLoop(self):
        while self.__isRunning:
            self.saveraw_mongo()
            if self.saveraw_csv():
                print(f"saved {len(self.rawdata)} packages")
                self.rawdata = []

    def saveraw_mongo(self):
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

    def shutdown(self):
        print_magenta("<shutting down Datalogger>\n")
        self.__isRunning = 0

class INTERFACE:
    def __init__(self):
        self.isRunning = 1
        self.server = None
        self.loop()

    def loop(self):
        print_yellow("\n<<Interface TCP Server>>\n")
        print_yellow("/? for help\n")
        while self.isRunning:
            try:
                self.Command(input("...\n"))
            except Exception as e:
                print(e)
                break

    def Command(self, command: str):
        if len(command)>1 and command[0] == "/":
            match command[1:]:
                case "?":
                    print_yellow("<Help>")
                    print(
                        """
/r start server
/s shutdown server
/i get network info
/q quit this file
"""
                    )
                case "r":
                    self.server = TCP_SERVER()
                case "s":
                    if hasattr(self, "server") and self.server is not None:
                        self.server.shutdown()
                        del self.server
                    else:
                        print_yellow("No Server running to shut down\n")
                case "i":
                    get_network_info()
                case "q":
                    print_yellow("<shuting down Interface>\n")
                    if hasattr(self, "server") and self.server is not None:
                        self.server.shutdown()
                        del self.server
                    self.isRunning = 0
                case _:
                    print_red(f"Unknown Command\n")


if __name__ == "__main__":
    interface = INTERFACE()
