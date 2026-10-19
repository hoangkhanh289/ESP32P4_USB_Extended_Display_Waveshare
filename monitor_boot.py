import serial
import time

s = serial.Serial('COM26', 115200, timeout=0.1)
print("Listening on COM26 for 10 seconds... (Please press RST button on board)")
t0 = time.time()
while time.time() - t0 < 10:
    data = s.readline()
    if data:
        print(data.decode('utf-8', errors='replace').rstrip())
s.close()
