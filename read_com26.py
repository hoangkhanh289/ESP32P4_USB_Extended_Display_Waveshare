import serial
import time

try:
    s = serial.Serial('COM26', 115200, timeout=0.5)
    print("Listening on COM26...")
    t0 = time.time()
    while time.time() - t0 < 3:
        data = s.readline()
        if data:
            print(data.decode('utf-8', errors='replace').rstrip())
    s.close()
except Exception as e:
    print(f"Error: {e}")
