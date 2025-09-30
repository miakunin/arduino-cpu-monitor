#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Инициализация LCD (адрес 0x27, 16 символов, 2 строки)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Переменные для хранения данных
float temperature = 0;
int cpuLoad = 0;
unsigned long lastUpdate = 0;

void setup() {
  // Инициализация Serial
  Serial.begin(9600);
  
  // Инициализация LCD
  lcd.init();
  lcd.backlight();
  
  // Начальный экран
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CPU Monitor v2.0");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  
  delay(2000);
  
  // Очищаем и показываем шаблон
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CPU Temp: -- C");
  lcd.setCursor(0, 1);
  lcd.print("Load: --%");
}

void loop() {
  // Проверяем наличие данных
  if (Serial.available() > 0) {
    // Читаем строку до символа новой строки
    String data = Serial.readStringUntil('\n');
    
    // Парсим данные (формат: "температура,загрузка")
    int commaIndex = data.indexOf(',');
    
    if (commaIndex > 0) {
      // Извлекаем температуру и загрузку
      String tempStr = data.substring(0, commaIndex);
      String loadStr = data.substring(commaIndex + 1);
      
      temperature = tempStr.toFloat();
      cpuLoad = loadStr.toInt();
      
      // Обновляем время последнего обновления
      lastUpdate = millis();
      
      // Обновляем дисплей
      updateDisplay();
    }
  }
  
  // Показываем предупреждение если данные не обновлялись 5 секунд
  if (millis() - lastUpdate > 5000 && lastUpdate != 0) {
    lcd.setCursor(15, 0);
    lcd.print("!");
  }
  
  delay(50);
}

void updateDisplay() {
  // Первая строка: CPU Temp: XX.X C
  lcd.setCursor(0, 0);
  lcd.print("CPU Temp: ");
  
  // Форматируем температуру
  if (temperature < 10) {
    lcd.print(" ");  // Дополнительный пробел для выравнивания
  }
  lcd.print(temperature, 1);  // 1 знак после запятой
  lcd.print(" C ");  // Пробел в конце для очистки
  
  // Вторая строка: Load: XX%
  lcd.setCursor(0, 1);
  lcd.print("Load: ");
  
  // Форматируем загрузку с выравниванием
  if (cpuLoad < 10) {
    lcd.print("  ");  // Два пробела для однозначных чисел
  } else if (cpuLoad < 100) {
    lcd.print(" ");   // Один пробел для двузначных
  }
  lcd.print(cpuLoad);
  lcd.print("%  ");  // Пробелы для очистки остатка строки
  
  // Опционально: добавляем индикатор уровня загрузки
  lcd.setCursor(13, 1);
  if (cpuLoad < 25) {
    lcd.print("[_]");
  } else if (cpuLoad < 50) {
    lcd.print("[=]");
  } else if (cpuLoad < 75) {
    lcd.print("[#]");
  } else {
    lcd.print("[!]");
  }
}
