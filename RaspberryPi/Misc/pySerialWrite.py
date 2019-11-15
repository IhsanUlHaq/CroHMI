import serial
import time

ser = serial.Serial(
    port='/dev/cu.usbmodem14101',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=None
    )

print('Serial Connected')

time.sleep(5)

print('writing on serial')

ser.write('x')

print('writing on serial')

time.sleep(3)

print('end')

