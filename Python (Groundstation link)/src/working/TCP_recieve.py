import socket

# Create a TCP/IP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the address and port
server_address = ('192.168.178.23', 8888)  # Replace with the PC's IP address
server_socket.bind(server_address)

# Listen for incoming connections
server_socket.listen(1)

while True:
    print('Waiting for a connection...')
    connection, client_address = server_socket.accept()
    print('Connection from:', client_address)

    # Receive the data in chunks
    while True:
        data = connection.recv(1024)
        if not data:
            break
        print('Received:', bin(data))

    # Close the connection
    connection.close()
