# server program
import socket

HOST = ''                 # Symbolic name meaning all available interfaces
PORT = 50007              # Arbitrary non-privileged port
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
s.listen(1)


def connectAndSend():
    conn, addr = s.accept()
    print(('Connected by', addr))
    while True:
        data = conn.recv(1024)
        if not data:
            break
        print (("data", data))
    conn.close()

while True:
    connectAndSend()
