# Detailed Wiring Guide

## Pin Identification

### LCD I2C Module Pins
- **VCC/VDD**: Power supply (5V)
- **GND/VSS**: Ground
- **SDA**: Serial Data Line (I2C)
- **SCL/SCK**: Serial Clock Line (I2C)

### Arduino Pro Micro I2C Pins
- **Pin 2**: SDA (Fixed hardware I2C)
- **Pin 3**: SCL (Fixed hardware I2C)

## Important Notes
1. Pro Micro uses different I2C pins than Arduino Uno (which uses A4/A5)
2. These pins are hardware I2C and cannot be changed
3. Multiple I2C devices can share the same bus
