import serial
import time
import osd

print (os.path.abspath(serial.__file__))
ser = serial.Serial(xonxoff=1)
ser.port = '/dev/ttyUSB0'
ser.timeout = 1
ser.open()


def pulse():
    numPulses = 0
    x = 0
    while x < 100000:
        if(ser.getCTS()):
            numPulses = numPulses + 1
        x = x + 1
    print (numPulses)

if ser.isOpen():
    print ('Open: ' + ser.port)
    while True:
        pulse()


