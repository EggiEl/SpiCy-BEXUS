import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file with correct delimiter and specifying data types
file_path = 'ESA_2W5_V2.csv'  # Path to your CSV file
data = pd.read_csv(file_path, delimiter=';', dtype=str)

# Convert the columns to numeric values, forcing errors to NaN
data['timestamp[ms]'] = pd.to_numeric(data['timestamp[ms]'].str.strip(), errors='coerce')
data['temp_probe[°C]'] = pd.to_numeric(data['temp_probe[°C]'].str.strip(), errors='coerce')
# data['temp_out[°C]'] = pd.to_numeric(data['temp_out[°C]'].str.strip(), errors='coerce')

# Convert timestamp to minutes for better readability (if needed)
data['timestamp[ms]'] = data['timestamp[ms]'] / 1000 / 60  # Convert ms to min

# Replace the placeholder value -1000000.00 with NaN for better plotting
data.replace(-1000000.00, float('nan'), inplace=True)

# Apply a rolling average
data['temp_probe[°C]'] = data['temp_probe[°C]'].rolling(window=20).mean()
# data['temp_out[°C]'] = data['temp_out[°C]'].rolling(window=20).mean()

# Use a clean style
plt.style.use('seaborn-v0_8')

# Plotting
plt.figure(figsize=(12, 8), dpi=300)  # Adjust size and resolution

# Plot with refined styles
plt.plot(data['timestamp[ms]'], data['temp_probe[°C]'], label='Temperature Probe (°C)', color='blue', linestyle='-', linewidth=1)
# plt.plot(data['timestamp[ms]'], data['temp_out[°C]'], label='Temperature Out (°C)', color='red', linestyle='-', linewidth=1)

# Enhance readability
plt.xlabel('Time (min)', fontsize=14)
plt.ylabel('Temperature (°C)', fontsize=14)
plt.title('Temperature over Time', fontsize=16)
plt.legend(fontsize=12)
plt.grid(True, which='both', linestyle='--', linewidth=0.5)

# Remove top and right spines
ax = plt.gca()
ax.spines['top'].set_visible(False)
ax.spines['right'].set_visible(False)

# Save the plot as a high-resolution image
plt.savefig('temperature_plot.png', dpi=1000, bbox_inches='tight')

# Display the plot
# plt.show()
