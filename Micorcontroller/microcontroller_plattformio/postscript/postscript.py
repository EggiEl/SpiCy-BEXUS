import shutil
import os

def copy_firmware(source_file, destination_dir):
    try:
        # Create the destination directory if it doesn't exist
        if not os.path.exists(destination_dir):
            os.makedirs(destination_dir)
        
        # Construct the destination file path
        destination_file = os.path.join(destination_dir, os.path.basename(source_file))
        
        # Copy the firmware file to the destination directory
        shutil.copy2(source_file, destination_file)
        
        print("Firmware copied successfully.")
    except Exception as e:
        print(f"An error occurred: {e}")
    

# Source file path
source_file = r"C:\Users\fgewi\Documents\PlatformIO\Projects\Pico Test\.pio\build\pico\firmware.uf2"

# Destination directory path
destination_directory = r"E:\\"

# Copy the firmware file to the destination directory
copy_firmware(source_file, destination_directory)
