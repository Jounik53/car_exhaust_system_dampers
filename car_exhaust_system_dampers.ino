//v1.1 Программа для управления заслонками, по штатной кнопке 12в 
//для автомобиля audi A4B8


const int relayOffPlus = 8; //Реле + (по умолчанию: 4)
const int relayOnPlus = 9;  //Реле + (по умолчанию: 5)
const int relayMinus = 7;   //Реле - По умолчанию минус на плюсе закрывающего положения (по умолчанию: 6)
const int relayDiod = 6;    //Реле управления светодиодом (по умолчанию: 8)
const int buttonPin = 2;    //Кнопка

bool flag_button = false;   //Положение кнопки, для отслеживания однократного нажатия
bool flag_state = false;    //Текущее положение заслонки 
uint32_t btnTimer = 0;      //Таймер для убирания дребезга

//Конфигурация
bool debug = true;         //Флаг для включения отладки в порт 
bool startup_sound = false; //Флаг для определения запуска на открытых или закрытых заслонках (false - закрытые)
int delay_mils = 2000;      //Величина задержки

void setup() {
  //Включение отладки
  if (debug) {
    Serial.begin(9600);
  }

  setupInputs();                    //Конфигурируем устройства
  setupRelay();                     //Конфигурация реле (выключение при запуске нагрузок)
  startupOptions(startup_sound);    //Конфигурация открытия/закрытия при запуске авто
}

void setupInputs() {
  //Настройка всех устройств
  pinMode(buttonPin, INPUT_PULLUP);  //Кнопка управления(с подтяжкой)

  pinMode(relayOffPlus, OUTPUT);     //Реле, настроенное на закрытие заслонок (плюсовой контакт)
  pinMode(relayOnPlus, OUTPUT);      //Реле, настроенное на открытие заслонок (плюсовой контакт)
  pinMode(relayMinus, OUTPUT);       //Реле, управляющее минусовым контактом для реле активации заслонок
  pinMode(relayDiod, OUTPUT);        //Реле, управляющее светодиодом, индикатором открытых заслонок
}

void setupRelay() {
  //Перевод реле в отключенный режим
  digitalWrite(relayOffPlus, HIGH);
  digitalWrite(relayOnPlus, HIGH);
  digitalWrite(relayMinus, HIGH);
  digitalWrite(relayDiod, HIGH);
}

void startupOptions(bool open_flag) {
  if (open_flag){
    printToSerial("Запуск на громкой");
    //Запуск на громкой машине
    digitalWrite(relayMinus, LOW);
    delay(10);
    digitalWrite(relayOffPlus, LOW);
    delay_function();
    digitalWrite(relayOffPlus, HIGH);
    digitalWrite(relayDiod, LOW);
  } else {
    printToSerial("Запуск на тихой");
    //запуск на тихой машине
    digitalWrite(relayOffPlus, LOW);
    delay_function();
    digitalWrite(relayOffPlus, HIGH);
  }
}

void printToSerial(String message) {
  if (debug) {
    Serial.println(message);
  }
}

void delay_function() {
  delay(delay_mils);
}

void change_state_relay(bool state) {
  if (state) {
      digitalWrite(relayMinus, LOW);
      printToSerial("Минусовое реле  открыто");
      delay(10);
      digitalWrite(relayOnPlus, LOW);
      printToSerial("Плюсовое реле на открытие открыто");
      delay_function();
      digitalWrite(relayOnPlus, HIGH);
      printToSerial("Плюсовое реле на открытие закрыто");
      digitalWrite(relayDiod, LOW);
      printToSerial("Громкий включен, индикация включена");
  } else {
      digitalWrite(relayMinus, HIGH);
      printToSerial("Минусовое реле  закрыто");
      digitalWrite(relayOffPlus, LOW);
      printToSerial("Плюсовое реле на закрытие открыто");
      delay_function();
      digitalWrite(relayOffPlus, HIGH);
      printToSerial("Плюсовое реле на закрытие закрыто");
      digitalWrite(relayDiod, HIGH);
      printToSerial("Тихий выхлоп влючен, индикация выключена");
  }
}

void loop() {
  bool btnState = digitalRead(buttonPin);
  if (btnState && !flag_button && millis() - btnTimer > 100) {
    flag_button = true;
    btnTimer = millis();
    printToSerial("Кнопка нажата");
    //Управление реле
    if (!flag_state) {
      printToSerial("Статус: открыть");
      change_state_relay(true);
      flag_state = !flag_state;
    } else {
      printToSerial("Статус: закрыть");      
      change_state_relay(false);
      flag_state = !flag_state;
    }
  }
  if (!btnState && flag_button && millis() - btnTimer > 100) {
    flag_button = false;
    btnTimer = millis();
  }
}
