#include <U8g2lib.h>
#include <Wire.h>

// Инициализация дисплея
U8G2_SSD1309_128X64_NONAME2_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Переменные для данных
char timeStr[6] = "00:00";
char secondsStr[3] = "00";
char dateStr[12] = "Loading";
float cpuTemp = 0;
int cpuLoad = 0;
unsigned long lastDataTime = 0;
bool dataReceived = false;

void setup() {
  delay(2000); // Защита от зависания при загрузке
  Serial.begin(9600);
  u8g2.begin();
  
  // Экран загрузки
  showLoadingScreen();
}

void loop() {
  // Читаем данные с Serial
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    if (data.length() > 0) {
      parseData(data);
      lastDataTime = millis();
      dataReceived = true;
    }
  }
  
  // Обновляем дисплей
  updateDisplay();
  delay(100); // Обновление 10 раз в секунду для плавности
}

void parseData(String data) {
  // Парсим формат: "HH:MM,SS,DD Mon,температура,загрузка"
  int idx1 = data.indexOf(',');
  int idx2 = data.indexOf(',', idx1 + 1);
  int idx3 = data.indexOf(',', idx2 + 1);
  int idx4 = data.indexOf(',', idx3 + 1);
  
  if (idx4 > 0) {
    // Извлекаем данные
    data.substring(0, idx1).toCharArray(timeStr, 6);
    data.substring(idx1 + 1, idx2).toCharArray(secondsStr, 3);
    data.substring(idx2 + 1, idx3).toCharArray(dateStr, 12);
    cpuTemp = data.substring(idx3 + 1, idx4).toFloat();
    cpuLoad = data.substring(idx4 + 1).toInt();
  }
}

void showLoadingScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(10, 30, "CPU Clock");
  u8g2.setFont(u8g2_font_7x13_tr);
  u8g2.drawStr(15, 50, "Waiting for data...");
  u8g2.sendBuffer();
}

void updateDisplay() {
  u8g2.clearBuffer();
  
  if (!dataReceived) {
    // Если данных еще нет
    showLoadingScreen();
    return;
  }
  
  // === ЧАСЫ (большие) ===
  u8g2.setFont(u8g2_font_logisoso32_tn); // Очень большой шрифт только для цифр
  u8g2.drawStr(2, 32, timeStr);
  
  // === СЕКУНДЫ (маленькие, справа от часов) ===
  u8g2.setFont(u8g2_font_profont15_tn); // Средний шрифт для секунд
  u8g2.drawStr(100, 25, ":");
  u8g2.drawStr(105, 25, secondsStr);
  
  // === ДАТА (под часами) ===
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(2, 42, dateStr);
  
  // === РАЗДЕЛИТЕЛЬНАЯ ЛИНИЯ ===
  u8g2.drawLine(0, 45, 128, 45);
  
  // === CPU ИНФОРМАЦИЯ (внизу) ===
  u8g2.setFont(u8g2_font_6x10_tr);
  
  // Температура слева
  u8g2.setCursor(2, 57);
  if (cpuTemp > 0) {
    u8g2.print("CPU: ");
    u8g2.print(cpuTemp, 1);
    u8g2.print("C");
  } else {
    u8g2.print("CPU: --.-C");
  }
  
  // Загрузка справа с прогресс-баром
  u8g2.setCursor(65, 57);
  u8g2.print("Load: ");
  u8g2.print(cpuLoad);
  u8g2.print("%");
  
  // Мини прогресс-бар под текстом загрузки
  int barWidth = map(cpuLoad, 0, 100, 0, 60);
  u8g2.drawFrame(65, 59, 62, 4);
  if (barWidth > 0) {
    u8g2.drawBox(66, 60, barWidth, 2);
  }
  
  // === ИНДИКАТОР СВЯЗИ ===
  // Мигающая точка если данные приходят
  if ((millis() - lastDataTime) < 2000) {
    if (millis() % 1000 < 500) {
      u8g2.drawCircle(124, 3, 2);
    }
  } else {
    // Если данных нет больше 2 секунд
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(100, 7, "LOST");
  }
  
  u8g2.sendBuffer();
}
```

### Результат на экране:
```
 ●                          [индикатор связи]
 14:32 :45                  [большие часы, маленькие секунды]
 30 Dec                     [дата]
─────────────────────────
 CPU: 45.2C   Load: 67%    [температура и загрузка]
              ▓▓▓▓▓░░░      [прогресс-бар]