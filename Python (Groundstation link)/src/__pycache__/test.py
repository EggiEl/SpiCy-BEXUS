import PySimpleGUI as sg
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# Define the layout
layout = [
    [sg.Canvas(size=(400, 200), key='-CANVAS-')],
    [sg.Button('Add Temperature'), sg.Button('Exit')]
]

# Create the window
window = sg.Window('Temperature Plot', layout,finalize=True)

# Initialize empty array for temperature data
temperature_data = []

# Create a Matplotlib figure and axis
fig, ax = plt.subplots()
line, = ax.plot([], [])

# Get the Matplotlib canvas
canvas_elem = window['-CANVAS-'].TKCanvas

# Display the Matplotlib plot on the PySimpleGUI window
canvas = FigureCanvasTkAgg(fig, master=canvas_elem)
canvas.draw()
canvas.get_tk_widget().pack(side='top', fill='both', expand=1)

# Event loop
while True:
    event, values = window.read()
    if event == sg.WINDOW_CLOSED or event == 'Exit':
        break
    elif event == 'Add Temperature':
        # Generate random temperature value
        new_temperature = np.random.uniform(0, 100)
        # Add the new temperature value to the array
        temperature_data.append(new_temperature)
        # Update the plot with the new temperature data
        line.set_xdata(np.arange(len(temperature_data)))
        line.set_ydata(temperature_data)
        ax.relim()
        ax.autoscale_view()
        canvas.draw()

# Close the window
window.close()
