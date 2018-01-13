from socket import *
import sys

sock = socket(AF_INET, SOCK_STREAM)
sock.bind(('',5642))
sock.listen(1)
csock, info = sock.accept()
def read():
    data=csock.recv(1000)
    return data
def write(str):
    csock.send(str)
