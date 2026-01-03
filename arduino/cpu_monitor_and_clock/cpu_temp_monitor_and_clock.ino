#include <U8g2lib.h>
#include <Wire.h>

// Display initialization
U8G2_SSD1309_128X64_NONAME2_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Variables for data
char timeStr[6] = "00:00";
char dateStr[12] = "Loading";
float cpuTemp = 0;
int cpuLoad = 0;
unsigned long lastDataTime = 0;
bool dataReceived = false;

void setup() {
  delay(2000); // Freezing protection 
  Serial.begin(9600);
  u8g2.begin();
  u8g2.enableUTF8Print();          // "°"
  
  // Boot display
  showLoadingScreen();
}

void loop() {
  // Reading data from Serial
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    if (data.length() > 0) {
      parseData(data);
      lastDataTime = millis();
      dataReceived = true;
    }
  }
  
  // Display update
  updateDisplay();
  delay(500); // Update display 2 Hz
}

void parseData(String data) {
  // Format parsing: "HH:MM,DD Mon, temp.,load"
  int idx1 = data.indexOf(',');           // После времени
  int idx2 = data.indexOf(',', idx1 + 1); // После даты
  int idx3 = data.indexOf(',', idx2 + 1); // После температуры
    
  if (idx1 > 0 && idx2 > 0 && idx3 > 0) {
    // extract data
    data.substring(0, idx1).toCharArray(timeStr, 6);        // "15:42"
    data.substring(idx1 + 1, idx2).toCharArray(dateStr, 12); // "30 Dec"
    cpuTemp = data.substring(idx2 + 1, idx3).toFloat();      // "45.2"
    cpuLoad = data.substring(idx3 + 1).toInt();              // "67"

  }
}

void showLoadingScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(10, 30, "CPU Clock");
  u8g2.setFont(u8g2_font_6x13_tr);
  u8g2.drawStr(7, 50, "Waiting for data...");
  u8g2.sendBuffer();
}

void updateDisplay() {
  u8g2.clearBuffer();
  
  if (!dataReceived) {
    // If there's no data transfer
    showLoadingScreen();
    return;
  }
  
  // === CLOCK (large) ===
  //u8g2.setFont(u8g2_font_logisoso32_tn); 
  //u8g2.drawStr(16, 32, timeStr);
  u8g2.setFont(u8g2_font_7_Seg_33x19_mn);
  u8g2.drawStr(15, 1, timeStr);
    
  // === Date ===
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(2, 44, dateStr);
    
  u8g2.drawLine(0, 47, 128, 47);
  
  // === CPU  ===
  //u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setFont(u8g2_font_6x12_tf);

  // Температура слева
  u8g2.setCursor(2, 60);
  if (cpuTemp > 0) {
    u8g2.print("CPU: ");
    u8g2.print(cpuTemp, 1);
    //u8g2.print("\xC2\xB0");  // "°" в UTF-8 
    u8g2.print("C");
  } else {
      u8g2.print("--.-");
      //u8g2.print("\xC2\xB0");
      u8g2.print("C");
  }
  
  // CPU load
  u8g2.setCursor(70, 60);
  u8g2.print("Load: ");
  u8g2.print(cpuLoad);
  u8g2.print("%");
    
  // === Connection indicator ===
  // Blinking dot if everyting's ok
  if ((millis() - lastDataTime) < 2000) {
    if (millis() % 1000 < 500) {
      //u8g2.drawCircle(124, 3, 2);  // disabled
    }
  } else {
    // If there is no data for longer that 2 s
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(106, 7, "LOST");
  }
  
  u8g2.sendBuffer();
}