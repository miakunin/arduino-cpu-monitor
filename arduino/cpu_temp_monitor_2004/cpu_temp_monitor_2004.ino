#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Инициализация LCD (адрес 0x27, 20 символов, 4 строки)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Переменные для хранения данных
float temperature = 0;
int cpuLoad = 0;
unsigned long lastUpdate = 0;

// Символы для прогресс-бара
byte bar0[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // пусто
byte bar1[8] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};  // 1/5
byte bar2[8] = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};  // 2/5
byte bar3[8] = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};  // 3/5
byte bar4[8] = {0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E};  // 4/5
byte bar5[8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};  // полный

void setup() {
  // Инициализация Serial
  Serial.begin(9600);
  
  // Инициализация LCD
  lcd.init();
  lcd.backlight();
  
  // Создаем пользовательские символы для прогресс-бара
  lcd.createChar(0, bar0);
  lcd.createChar(1, bar1);
  lcd.createChar(2, bar2);
  lcd.createChar(3, bar3);
  lcd.createChar(4, bar4);
  lcd.createChar(5, bar5);
  
  // Начальный экран
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("CPU Monitor v2.0");
  lcd.setCursor(4, 2);
  lcd.print("Starting...");
  
  delay(2000);
  
  // Очищаем и показываем шаблон
  lcd.clear();
  
  // Строка 0: Заголовок
  lcd.setCursor(0, 0);
  lcd.print("==== CPU STATS ====");
  
  // Строка 1: Температура
  lcd.setCursor(0, 1);
  lcd.print("Temp: -- C");
  
  // Строка 2: Загрузка
  lcd.setCursor(0, 2);
  lcd.print("Load: --%");
  
  // Строка 3: Прогресс-бар (подпись)
  lcd.setCursor(0, 3);
  lcd.print("[");
  lcd.setCursor(19, 3);
  lcd.print("]");
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
    lcd.setCursor(19, 0);
    lcd.print("!");
  } else if (lastUpdate != 0) {
    lcd.setCursor(19, 0);
    lcd.print("=");
  }
  
  delay(50);
}

void updateDisplay() {
  // Строка 1: CPU Temp: XX.X C
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  
  // Форматируем температуру
  if (temperature < 10) {
    lcd.print(" ");  // Дополнительный пробел для выравнивания
  }
  lcd.print(temperature, 1);  // 1 знак после запятой
  
  // Добавляем символ градуса и C
  lcd.print((char)223);  // символ градуса °
  lcd.print("C ");
  
  // Индикатор температуры
  if (temperature > 80) {
    lcd.print("HOT! ");
  } else if (temperature > 70) {
    lcd.print("WARM ");
  } else if (temperature > 50) {
    lcd.print("NORM ");
  } else {
    lcd.print("COOL ");
  }
  
  // Строка 2: Load: XX%
  lcd.setCursor(0, 2);
  lcd.print("Load: ");
  
  // Форматируем загрузку с выравниванием
  if (cpuLoad < 10) {
    lcd.print("  ");  // Два пробела для однозначных чисел
  } else if (cpuLoad < 100) {
    lcd.print(" ");   // Один пробел для двузначных
  }
  lcd.print(cpuLoad);
  lcd.print("%");
  
  // Индикатор уровня загрузки текстом
  lcd.setCursor(14, 2);
  if (cpuLoad < 25) {
    lcd.print(" IDLE ");
  } else if (cpuLoad < 50) {
    lcd.print(" LOW  ");
  } else if (cpuLoad < 75) {
    lcd.print(" MED  ");
  } else {
    lcd.print(" HIGH ");
  }
  
  // Строка 3: Прогресс-бар загрузки
  drawProgressBar(3, 1, 18, cpuLoad);
}

// Функция отрисовки прогресс-бара
void drawProgressBar(int row, int startCol, int length, int percent) {
  // Ограничиваем процент от 0 до 100
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  
  // Вычисляем сколько "пикселей" нужно заполнить (5 пикселей на символ)
  int totalPixels = length * 5;
  int filledPixels = (totalPixels * percent) / 100;
  
  // Отрисовываем прогресс-бар
  lcd.setCursor(startCol, row);
  
  for (int i = 0; i < length; i++) {
    int pixelsInThisChar = filledPixels - (i * 5);
    
    if (pixelsInThisChar >= 5) {
      lcd.write(5);  // Полностью заполненный символ
    } else if (pixelsInThisChar > 0) {
      lcd.write(pixelsInThisChar);  // Частично заполненный
    } else {
      lcd.write(0);  // Пустой символ
    }
  }
}
