import socket
import time

# Create a TCP/IP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the address and port
server_address = ('', 8888)  # Listen on all available interfaces
server_socket.bind(server_address)

# Listen for incoming connections
server_socket.listen(1)

print('Waiting for a connection...')
connection, client_address = server_socket.accept()
print('Connection from:', client_address)

try:
    while True:
        # Send data to the client
        message = b'Hello, Arduino!'
        print('Sending:', message)
        connection.sendall(message)

        # Wait for a brief moment before sending the next message
        time.sleep(1)

finally:
    # Close the connection
    connection.close()
