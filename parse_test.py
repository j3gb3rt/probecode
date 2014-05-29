import time


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
        print((reformat_string(curr_line)))  # lint:ok
