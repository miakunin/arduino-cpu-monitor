#!/usr/bin/env python3
import serial
import serial.tools.list_ports
import time
import os
import glob
import psutil  # Для загрузки CPU

def find_arduino():
    """Автоматический поиск Arduino Pro Micro"""
    ports = serial.tools.list_ports.comports()
    
    for port in ports:
        # Проверяем по описанию
        if any(x in port.description.lower() for x in ['arduino', 'pro micro', 'sparkfun']):
            print(f"Найден Arduino: {port.device} - {port.description}")
            return port.device
            
        # Проверяем по VID:PID
        if port.vid and port.pid:
            if (port.vid == 0x2341 or port.vid == 0x1b4f or port.vid == 0x2a03):
                print(f"Найден по VID:PID: {port.device}")
                return port.device
    
    # Если не нашли по ID, ищем любой ttyACM или ttyUSB
    for port in ports:
        if 'ttyACM' in port.device or 'ttyUSB' in port.device:
            print(f"Найден Serial порт: {port.device}")
            return port.device
    
    return None

def get_cpu_temperature():
    """Получение температуры CPU"""
    try:
        # Ищем x86_pkg_temp
        for zone_path in glob.glob('/sys/class/thermal/thermal_zone*/type'):
            with open(zone_path, 'r') as f:
                if 'x86_pkg_temp' in f.read():
                    temp_path = zone_path.replace('/type', '/temp')
                    with open(temp_path, 'r') as tf:
                        return int(tf.read().strip()) / 1000
    except:
        pass
    
    # Если не нашли x86_pkg_temp, используем thermal_zone0
    try:
        with open('/sys/class/thermal/thermal_zone0/temp', 'r') as f:
            return int(f.read().strip()) / 1000
    except:
        pass
    
    return None

def get_cpu_load():
    """Получение загрузки CPU в процентах"""
    # interval=1 означает измерение за 1 секунду
    return psutil.cpu_percent(interval=1)

def main_loop():
    """Основной цикл с автопереподключением"""
    arduino = None
    last_port = None
    
    while True:
        try:
            # Ищем Arduino если не подключена
            if arduino is None or not arduino.is_open:
                port = find_arduino()
                
                if port is None:
                    print("Arduino не найден, ожидание...")
                    time.sleep(2)
                    continue
                
                # Подключаемся
                print(f"Подключение к {port}...")
                arduino = serial.Serial(port, 9600, timeout=1)
                last_port = port
                time.sleep(2)  # Ждем инициализацию
                print("Подключено!")
            
            # Проверяем что порт все еще существует
            if not os.path.exists(last_port):
                print("Порт отключен")
                arduino.close()
                arduino = None
                continue
            
            # Получаем данные
            temp = get_cpu_temperature()
            load = get_cpu_load()
            
            if temp is not None:
                # Формат: "температура,загрузка"
                message = f"{temp:.1f},{load:.0f}"
                arduino.write((message + '\n').encode())
                print(f"\rCPU: {temp:.1f}°C, Load: {load:.0f}%    ", end='', flush=True)
            else:
                # Если нет данных о температуре, отправляем только загрузку
                message = f"0,{load:.0f}"
                arduino.write((message + '\n').encode())
                print(f"\rCPU: No temp, Load: {load:.0f}%    ", end='', flush=True)
            
            # Загрузка CPU уже измеряется 1 секунду в get_cpu_load()
            # поэтому дополнительная задержка не нужна
            
        except serial.SerialException as e:
            print(f"\nОшибка Serial: {e}")
            if arduino:
                arduino.close()
            arduino = None
            time.sleep(2)
        except KeyboardInterrupt:
            print("\nОстановка...")
            break
        except Exception as e:
            print(f"\nНеожиданная ошибка: {e}")
            time.sleep(2)
    
    if arduino and arduino.is_open:
        arduino.close()

if __name__ == "__main__":
    print("CPU Monitor v2.0 - Temperature + Load")
    print("=" * 40)
    
    # Проверяем установлен ли psutil
    try:
        import psutil
    except ImportError:
        print("ОШИБКА: Библиотека psutil не установлена!")
        print("Установите: sudo apt install python3-psutil")
        print("Или: pip3 install psutil")
        exit(1)
    
    main_loop()
