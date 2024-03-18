import os
import sys
import time
import serial
# import serial.tools.list_ports
import threading
import PySimpleGUI as psg


class GUI:
	def __init__(self):
		psg.theme('DarkAmber')
		layout = [
			[psg.Text("Comports ",key='COM')],
			[psg.Text("Address "), psg.Input()],
			[psg.Text("Email ID "), psg.Input()],
			[psg.OK(), psg.Cancel()],
		]

		window = psg.Window('Spicy_Groundstation', layout, size=(715,207))
		
		
		string = ""
		for a in listComports():
			string += str(a) 
		event, values = window.read()
		print(string)
		

		
		print(event, values)


def listComports() -> list:
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


def logCom():
	def readinlog(port):
		while 1:
			data = port.readline()[:-2]
			if data:
				print(str(data)[2:-1])

	ports = listComports()
	for port in ports:
		x = threading.Thread(target=readinlog, args=(port,))
		x.start()


if __name__ == "__main__":
	# logCom()
	# gui = GUI()
	pass 