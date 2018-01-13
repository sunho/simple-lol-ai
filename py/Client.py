import socket
import sys

sock = socket.create_connection(('172.30.1.31', 5625))
def read():
    data=sock.recv(1000)
    return data.decode()
def write(str):
    sock.send(str.encode())
