import socket
import struct
import threading
import time
from datetime import datetime

def millis():
	return round(time.time() * 1000)


class TCP_SERVER:
	def __init__(self):
		self.rawdata = []
		self.ipadress = "192.168.178.23"
		self.port = 8888
		self.isRunning = 1
		self.thread = threading.Thread(target=self.StartServer)
		self.thread.start()

	def StartServer(self):
		print("starting TCP server...")
		server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Erstellen Sie einen Socket für den Server
		
		server_socket.bind((self.ipadress, self.port))  # Binden Sie den Socket an den Host und Port
		server_socket.listen(1)  # Warten Sie auf eingehende Verbindungen
		client_socket, client_address = (server_socket.accept())  # Akzeptieren Sie eine Verbindung
		while self.isRunning:
			# print('Server wartet auf Verbindung..',end='')
			# print(' | Verbindung hergestellt von:', client_address)

			received_data = b""  # Empfangen Sie die Daten als Zeichenarray

			client_socket.settimeout(2)
			try:
				data_chunk = client_socket.recv(1024)
				if not data_chunk:
					break
				received_data += data_chunk
			except socket.timeout:
				print(f"Socket timed out while waiting for {client_address}. Waiting for new connections")
				client_socket.close()  # Schließen Sie die Verbindung zum Client
				server_socket.listen(1)  # Warten Sie auf eingehende Verbindungen
				client_socket, client_address = (server_socket.accept())  # Akzeptieren Sie eine Verbindung
				print(f'new connection with {client_address}')
	
			# print(f'Recieved Data: {received_data}')
			print(f'recieved: {len(received_data)} bytes from {client_address}')
#			 time.time()
			self.rawdata.append((received_data,time.time()))

		client_socket.close()  # Schließen Sie die Verbindung zum Client
		server_socket.close()  # Schließen Sie den Server-Socket

class DATALOGGER:
	"""datalogger takes data from listener and saves it in corresponding files"""
	def __init__(self, _struct_format="L L 6L 6f 6f 6i i f 2i 80s") -> None:
		self.rawdata = "rawdata.csv"
		self.formateddata = "data.csv"
		self.struct_format = _struct_format
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

		packets = []
		for el,timestamp in TCP_server.rawdata:
			# Format the datetime object as a human-readable string
			human_readable_time = datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d %H:%M:%S')
			try:
					unpacked_data = struct.unpack(self.struct_format, el)  # Entpacken Sie die Daten in die Struktur
					packets.append((unpacked_data,human_readable_time))
			except:
				print("struct_format is wrong or data is corrupted")
				print(f"Length of Recieved Data: {len(el)}")
				print(f"Recieved Data: {el}")
				print()

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
