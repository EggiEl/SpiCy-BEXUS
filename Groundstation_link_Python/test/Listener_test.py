import time
import serial
import serial.tools.list_ports
from threading import Thread
import PySimpleGUI as psg

from working import Groundstation

class LISTENER:
    pass

def test_answer():
    rp2040 = serial.Serial(port='COM8', baudrate=9600, timeout=.1) 
    a = LISTENER(rp2040)
    time.sleep(5)
    print(a)
    a.stop()
    assert 2 == 5