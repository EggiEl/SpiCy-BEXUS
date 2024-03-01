import sys

# Print a message
print("Deleting this line in 3 seconds...", end='', flush=True)

# Simulate some processing time
import time
time.sleep(3)

# Delete the last printed message
print("\r" + " " * 70 + "\r", end='', flush=True)

# Print a new message
print(f'recieved s', end='', flush=True)
