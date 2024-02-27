import os
import sys
import time
import serial
import random
import serial.tools.list_ports
import threading
import PySimpleGUI as psg
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np

class LISTENER:
	"""opens a COM Port in a Thread and prints everything it contains in a list witch a timestamp"""
	def __init__(self,_port : serial.Serial):
		self.port = _port
		self.port.timeout = 1
		self.name = _port.name
		self.running = 1
		self.data = []
		self.thread = threading.Thread(target=self.readinlog)
		self.thread.start()
	
	"""listens to the port and read in evrything with a timestamp"""
	def readinlog(self):
		while self.running:
			packet = self.port.readline()[:-2] #hängt wenn nichts übtragen wird -> thread is blocked
			if packet:
				self.data.append((time.time(),str(packet)[2:-1]))

	"""kills the listener"""
	def kill(self):
		self.running = 0
		while self.thread.is_alive():
			self.thread.join()
	
	def __str__(self) -> str:
		string = f'{self.port.name}\n'
		for time,data in self.data:
			string += f't: {time/1000} | dunjzmökl : {data}\n'
		return string

class INTERFACE:
	def __init__(self):
		self.ports = self.listComports()
		self.portnames = [a.name for a in self.ports]
		self.listeners = self.attachlisteners()
		
	def listComports(self) -> list[serial.Serial]:
		print("Search...")
		ports = serial.tools.list_ports.comports(include_links=False)
		openport = []
		for port in ports:
			try:
				ser = serial.Serial(port.device)
				if ser.is_open:
					openport.append(ser)
			except:
				pass
		[print(a) for a in openport]
		return openport
	
	def attachlisteners(self):
		return [LISTENER(port) for port in self.ports]
	
	def killlisteners(self):
		for i in self.listeners:
			i.kill()
			
	def getdata(self,portname:str):
		for listener in self.listeners:
			if listener.name == portname:
				return str(listener)
				break

class DATAHANDLER:
	def __init__(self,interface = INTERFACE) -> None:
		self.temperature_data = []
		self.oxygen_data = []
		self.oxygen_data.append(np.random.uniform(0, 100, size=100))
		self.temperature_data = [np.random.normal(loc=20, scale=5, size=100) for _ in range(6)]
		# self.oxygen_data= [np.random.uniform(0, 100, size=100) for _ in range(6)]
		self.running = 1
		self.thread = threading.Thread(target=self.simulateDataIn)
		
		# self.thread.start()
	
	def simulateDataIn(self):
		while(self.running):
			for i in range(6):
				# self.temperature_data[i].append(25 + (random.random()-0.5)*2)
				# self.oxygen_data[i].append(random.random())
				self.oxygen_data[i].np.append(self.oxygen_data[i], new_data_point)
			time.sleep(1)

	

class GUI():
	def __init__(self):
		self.running = 1
		self.interface = INTERFACE()
		self.datahandler = DATAHANDLER(self.interface)

		self.window = psg.Window('Spicy_Groundstation', self.generatelayout(), resizable=True, finalize=True)
		
		self.time = np.linspace(0, 10, 100)

		self.figures = []
		self.canvas_widgets = []
		self.loop()

	def loop(self):
		self.create_graphs()
		updatecom = threading.Thread(target=self.updateField)
		updatecom.start()
		self.window['-COMSEL-'].update(values=self.interface.portnames)
		while True:
			event, values = self.window.read()
			match event:
				case psg.WIN_CLOSED:
					self.interface.killlisteners()
					self.datahandler.running = 0
					self.running = 0
					self.window.close()
					sys.exit()
				case "Refresh":	
					"""updates the Com Port selection"""
					self.window['-COMSEL-'].update(values=self.interface.portnames)
				case 'Senden':
					self.window['-OUTPUT-'].print(values['-INPUT-'], end='\n')
				case _:
					pass

	def updateField(self):
		"""this runs in a thread and updates all fiels on the GUI, like the COM and Sensors"""

		while self.running:
			self.updateSensor()
			self.updateCom()
			time.sleep(0.5)

	def updateSensor(self):
		"""updates the plots of the sensor with fresh data"""
		for i in range(6):
			axes = self.figures[i].get_axes()
			axes[0].clear()
			axes[0].plot(self.time, self.datahandler.temperature_data[i], label='Temperature')
			axes[0].plot(self.time, self.datahandler.oxygen_data[i], label='Oxygen')
			axes[0].set_xlabel('Time')
			axes[0].set_ylabel('Values')
			axes[0].set_title(f'Graph {i+1}')
			axes[0].legend()
			self.canvas_widgets[i].draw()

	def create_graphs(self,sizepergraph=(4,4)):
		"""creates the canvases and figures for the sensor data"""
		temperature_data = [np.random.normal(loc=20, scale=5, size=100) for _ in range(6)]
		oxygen_data = [np.random.uniform(0, 100, size=100) for _ in range(6)]

		for i in range(2):
			for j in range(3):
				fig, ax = plt.subplots(figsize=sizepergraph)
				ax.plot(self.time, temperature_data[3*i+j], label='Temperature')
				ax.plot(self.time, oxygen_data[3*i+j], label='Oxygen')
				ax.set_xlabel('Time')
				ax.set_ylabel('Values')
				ax.set_title(f'Graph {3*i+j+1}')
				ax.legend()
				self.figures.append(fig)

				canvas_elem = self.window[f'-CANVAS{3*i+j}-'].TKCanvas
				canvas = FigureCanvasTkAgg(fig, master=canvas_elem)
				canvas.draw_idle()
				canvas_widget = canvas.get_tk_widget()
				canvas_widget.pack(side='top', fill='both', expand=1)
				self.canvas_widgets.append(canvas)


	def updateCom(self):
		"""updates the Serial Monitor from the data of the selected COM Port"""
		selectedCom =  self.window['-COMSEL-'].get()
		if len(selectedCom):
			self.window['-Serial_Monitor-'].update(value=self.interface.getdata(selectedCom[0]))

	def generatelayout(self):
		psg.theme('DarkAmber')
		sensors = psg.Frame('Sensor Data', layout=[
			[psg.Canvas(size=(400, 300), key=f'-CANVAS{i}-') for i in range(3)],
			[psg.Canvas(size=(400, 300), key=f'-CANVAS{i+3}-') for i in range(3)]
		])

		com =  psg.Frame('COMs', layout=[
			[psg.Frame('Serial_Plotter',layout=([[psg.Graph(canvas_size=(20, 20),graph_bottom_left=(0, 0),graph_top_right=(400, 200),key=f'-GRAPHCOM-',border_width= 10)]]))],
			[psg.Frame('Serial_Monitor',layout=([[psg.Multiline(default_text="no serial data yet",key= '-Serial_Monitor-', size=(50, 20), autoscroll=True)]]))],
			[psg.Listbox(values=["no COM Port detected"], size=(20, 5), key='-COMSEL-', enable_events=True),psg.Button('Refresh')]
		])

		errors =  psg.Frame('Errors', layout=[
			[psg.Multiline(default_text="hier könnte ihre Werbungs stehen", size=(40, 10), autoscroll=True)]
		])

		layout = [
			[sensors]+[errors]+[com]
		]
		return layout
	
if __name__ == "__main__":
	# rp2040 = serial.Serial(port='COM8', baudrate=9600, timeout=.1) 
	# gui = GUI()

	# i = INTERFACE()
	# print(i.getdata("COM8"))
	# print(i.portnames)
	# threading.current_thread()
	# i.killlisteners()

	# i = INTERFACE()
	# d = DATAHANDLER(i)
	# while True:
	# 	print(d.oxygen_data)
	# 	time.sleep(1)

