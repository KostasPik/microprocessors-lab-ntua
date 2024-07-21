import serial

ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=10)
buf = ser.readline()
print(buf)
ser.write(b'"Success"\n')
buf = ser.readline()
print(buf)
ser.write(b'"Success"\n')

