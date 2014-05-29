# client program
import socket
import time

#HOST = 'www.angryhangar.com'    # The remote host
HOST = '127.0.0.1'
PORT = 50007              # The same port as used by the server


def reformat_string(str):
    comma_count = 0
    ret = ""
    for i in str:
        if comma_count < 3 or comma_count == 4:
            ret += i
        if i == ',':
            comma_count += 1
    return ret[:-1]


f = open("/home/j3gb3rt/.wine/drive_c/users/j3gb3rt" +
         "/Desktop/Inspeed Data.csv", "r")
curr_line = "Go!"
data_start = 0
data_next = 0

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

while True:
    curr_line = f.readline()
    if curr_line == "":
        break
    data_start = data_next
    data_next = f.tell()
#    if curr_line[:1] == "*":
#        data_start = f.tell()
#        print(data_start)
f.seek(data_start)
while True:
    curr_line = f.readline()
    if (curr_line == ""):
        time.sleep(2)
    else:
        s.sendall(reformat_string(curr_line))
        print((reformat_string(curr_line)))  # lint:ok

#s.sendall(b'Hello, world')
#time.sleep(3)
#s.sendall(b'hi again')
s.close()
