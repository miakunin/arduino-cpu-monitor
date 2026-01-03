#/usr/bin/env python3
import serial
import serial.tools.list_ports
import time
import os
import glob
import psutil
from datetime import datetime

def find_arduino():
    """Automatically search for Arduino Pro Micro"""
    ports = serial.tools.list_ports.comports()
    
    for port in ports:
        if any(x in port.description.lower() for x in ['arduino', 'pro micro', 'sparkfun']):
            print(f"Arduino found: {port.device} - {port.description}")
            return port.device
            
        if port.vid and port.pid:
            if (port.vid == 0x2341 or port.vid == 0x1b4f or port.vid == 0x2a03):
                print(f"Found via VID:PID: {port.device}")
                return port.device
    
    for port in ports:
        if 'ttyACM' in port.device or 'ttyUSB' in port.device:
            print(f"Found Serial port: {port.device}")
            return port.device
    
    return None

def get_cpu_temperature():
    """Get CPU temperature"""
    try:
        for zone_path in glob.glob('/sys/class/thermal/thermal_zone*/type'):
            with open(zone_path, 'r') as f:
                if 'x86_pkg_temp' in f.read():
                    temp_path = zone_path.replace('/type', '/temp')
                    with open(temp_path, 'r') as tf:
                        return int(tf.read().strip()) / 1000
    except:
        pass
    
    try:
        with open('/sys/class/thermal/thermal_zone0/temp', 'r') as f:
            return int(f.read().strip()) / 1000
    except:
        pass
    
    return None

def get_cpu_load():
    """Get CPU load"""
    return psutil.cpu_percent(interval=1)

def main_loop():
    """Main loop with auto reconnect"""
    arduino = None
    last_port = None
    
    while True:
        try:
            if arduino is None or not arduino.is_open:
                port = find_arduino()
                
                if port is None:
                    print("Arduino not found, waiting...")
                    time.sleep(2)
                    continue
                
                print(f"Connected to {port}...")
                arduino = serial.Serial(port, 9600, timeout=1)
                last_port = port
                time.sleep(2)
                print("Connected! Sending data...")
            
            if not os.path.exists(last_port):
                print("Port has been disconnected")
                arduino.close()
                arduino = None
                continue
            
            # Getting time
            now = datetime.now()
            time_str = now.strftime("%H:%M")
            date_str = now.strftime("%d %b")
            
            # Getting CPU info
            temp = get_cpu_temperature()
            load = get_cpu_load()
            
            # If there in no temperature data using 0
            if temp is None:
                temp = 0
            
            # Format: "HH:MM,DD Mon,temperature,load"
            message = f"{time_str},{date_str},{temp:.1f},{load:.0f}"
            arduino.write((message + '\n').encode())
            
            # Print to the terminal
            print(f"\r{time_str} | CPU: {temp:.1f}°C, Load: {load:.0f}%    ", 
                  end='', flush=True)
            
            time.sleep(1)
            
        except serial.SerialException as e:
            print(f"\nError Serial: {e}")
            if arduino:
                arduino.close()
            arduino = None
            time.sleep(2)
        except KeyboardInterrupt:
            print("\nStop...")
            break
        except Exception as e:
            print(f"\nUnexpected error: {e}")
            time.sleep(2)
    
    if arduino and arduino.is_open:
        arduino.close()

if __name__ == "__main__":
    print("CPU Monitor v3.0 - Clock + Temperature + Load")
    print("=" * 50)
    
    try:
        import psutil
    except ImportError:
        print("ERROR: psutil lib is not found!")
        print("Please install: sudo apt install python3-psutil")
        print("Or: pip3 install psutil")
        exit(1)
    
    main_loop()
