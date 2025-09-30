# Arduino CPU Monitor

Real-time CPU temperature and load monitoring on LCD display via Arduino Pro Micro.

![Project Photo](docs/images/result.jpg)

## Features

- 🌡️ Real-time CPU temperature monitoring
- 📊 CPU load percentage display
- 🔌 Auto-reconnect on USB disconnect
- 🚀 Automatic startup with system
- 🔍 Auto-detection of Arduino port

## Hardware Requirements

- Arduino Pro Micro (or compatible ATmega32U4 board)
- LCD 1602 I2C Display (16x2 characters)
- 4 jumper wires
- USB cable

## Wiring

| LCD I2C | Arduino Pro Micro |
|---------|------------------|
| VCC     | VCC (5V)        |
| GND     | GND             |
| SDA     | Pin 2           |
| SCL     | Pin 3           |

![Wiring Diagram](docs/images/wiring.png)

## Installation

### 1. Arduino Setup

1. Open `arduino/cpu_monitor/cpu_monitor.ino` in Arduino IDE
2. Install library: `LiquidCrystal I2C` by Marco Schwartz
3. Select board: `Arduino Leonardo` or `SparkFun Pro Micro`
4. Upload sketch

### 2. Python Setup

**Install dependencies**
  sudo apt install python3-serial python3-psutil

**Or using pip**
  pip3 install -r requirements.txt

## Run

**Test run**
  python3 python/cpu_temp_monitor.py

**Install as service (optional)**
  sudo cp systemd/cpu-lcd-monitor.service /etc/systemd/system/
  sudo systemctl daemon-reload
  sudo systemctl enable cpu-lcd-monitor
  sudo systemctl start cpu-lcd-monitor

### Usage

The display shows:

┌──────────────────┐
│CPU Temp: 45.2 C  │
│Load: 23%    [=]  │
└──────────────────┘

**Load Indicators:**
- `[_]` = 0-25% (Idle)
- `[=]` = 25-50% (Normal)
- `[#]` = 50-75% (Busy)
- `[!]` = 75-100% (High)

## 🚀 Installation

### Step 1: Clone the Repository

  git clone https://github.com/yourusername/arduino-cpu-monitor.git
  cd arduino-cpu-monitor

### Step 2: Arduino Setup

  1. Open Arduino IDE
  2. Install Required Library

    - Go to Tools → Manage Libraries
    - Search for LiquidCrystal I2C
    - Install the one by Marco Schwartz or Frank de Brabander

  3. Configure Board

    - Tools → Board → Arduino Leonardo (or SparkFun Pro Micro)
    - Tools → Port → /dev/ttyACM0 (or your port)

  4. Upload Sketch

**Open the sketch**
  arduino/cpu_monitor/cpu_monitor.ino

**Click Upload (→) or press Ctrl+U**

### Step 3: Python Dependencies

**Ubuntu/Debian (recommended)**
  sudo apt install python3-serial python3-psutil

**Or using pip**
  pip3 install -r requirements.txt

**Add user to dialout group for USB access**
  sudo usermod -a -G dialout $USER
**Log out and back in for this to take effect**

### Step 4. Test Run

**Make script executable**
  chmod +x python/cpu_monitor.py

**Run the monitor**
  python3 python/cpu_monitor.py

You should see:

  CPU Monitor v2.0 - Temperature + Load
  ========================================
  Found Arduino: /dev/ttyACM0 - SparkFun Pro Micro
  Connecting to /dev/ttyACM0...
  Connected!
  CPU: 45.2°C, Load: 23%

### Step 5. Install as System Service (Optional)

**Copy service file**
  sudo cp systemd/cpu-lcd-monitor.service /etc/systemd/system/

**Edit the service file to match your paths**
  sudo nano /etc/systemd/system/cpu-lcd-monitor.service

**Enable and start service**
    sudo systemctl daemon-reload
    sudo systemctl enable cpu-lcd-monitor.service
    sudo systemctl start cpu-lcd-monitor.service

**Check status**
    sudo systemctl status cpu-lcd-monitor.service

## Configuration

### Change Temperature Sensor

Edit python/cpu_monitor.py, find the get_cpu_temperature() function:

**Default: uses x86_pkg_temp (CPU package temperature)**
**Alternative: use specific thermal zone**
    with open('/sys/class/thermal/thermal_zone0/temp', 'r') as f:
      return int(f.read().strip()) / 1000

      Adjust Update Interval
      The default update interval is 1 second. The CPU load measurement takes 1 second, so this is optimal. To change it, modify the interval parameter in get_cpu_load().
      Change I2C Address
      If your LCD has a different I2C address (some use 0x3F instead of 0x27):
      // In Arduino sketch, change:
      LiquidCrystal_I2C lcd(0x27, 16, 2);
      // To:
      LiquidCrystal_I2C lcd(0x3F, 16, 2);
