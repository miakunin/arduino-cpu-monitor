#include <U8g2lib.h>
#include <Wire.h>

// Инициализация дисплея
U8G2_SSD1309_128X64_NONAME2_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Переменные для данных
char timeStr[6] = "00:00";
char dateStr[12] = "Loading";
float cpuTemp = 0;
int cpuLoad = 0;
unsigned long lastDataTime = 0;
bool dataReceived = false;

void setup() {
  delay(2000);
  Serial.begin(9600);
  u8g2.begin();
  u8g2.enableUTF8Print();          // "°"


  showLoadingScreen();
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    if (data.length() > 0) {
      parseData(data);
      lastDataTime = millis();
      dataReceived = true;
    }
  }
  
  updateDisplay();
  delay(500);
}

void parseData(String data) {
  int idx1 = data.indexOf(',');
  int idx2 = data.indexOf(',', idx1 + 1);
  int idx3 = data.indexOf(',', idx2 + 1);
  
  if (idx1 > 0 && idx2 > 0 && idx3 > 0) {
    data.substring(0, idx1).toCharArray(timeStr, 6);
    data.substring(idx1 + 1, idx2).toCharArray(dateStr, 12);
    cpuTemp = data.substring(idx2 + 1, idx3).toFloat();
    cpuLoad = data.substring(idx3 + 1).toInt();
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
  
  // Проверяем таймаут данных
  bool connectionLost = (millis() - lastDataTime) > 2000;
  
  if (connectionLost) {
    // === ПОКАЗЫВАЕМ "NO DATA" ВМЕСТО ЧАСОВ ===
    u8g2.setFont(u8g2_font_ncenB14_tf);
    u8g2.drawStr(16, 25, "NO DATA");
    
    // Последнее время внизу мелким шрифтом
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(2, 45, "Last:  ");
    u8g2.drawStr(38, 45, timeStr);
    u8g2.drawStr(76, 45, dateStr);
    
  } else {
    // === НОРМАЛЬНЫЙ РЕЖИМ - ПОКАЗЫВАЕМ ЧАСЫ ===
    u8g2.setFont(u8g2_font_7_Seg_33x19_mn);
    u8g2.drawStr(15, 1, timeStr);

    // Дата под часами
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(2, 45, dateStr);
  }
  
  // === РАЗДЕЛИТЕЛЬНАЯ ЛИНИЯ ===
  u8g2.drawLine(0, 48, 128, 48);
  
  // === CPU ИНФОРМАЦИЯ ===
  u8g2.setFont(u8g2_font_6x12_tf);
  
  if (connectionLost) {
    // При потере связи показываем прочерки
    u8g2.setCursor(2, 61);
    u8g2.print("CPU: --.-C");
    
    u8g2.setCursor(70, 61);
    u8g2.print("Load: --%");
  } else {
    // Нормальный режим
    u8g2.setCursor(2, 61);
    if (cpuTemp > 0) {
      u8g2.print("CPU: ");
      u8g2.print(cpuTemp, 1);
      u8g2.print("C");
    } else {
      u8g2.print("CPU: --.-C");
    }
    
    u8g2.setCursor(70, 61);
    u8g2.print("Load: ");
    u8g2.print(cpuLoad);
    u8g2.print("%");
  }
  
  u8g2.sendBuffer();
}