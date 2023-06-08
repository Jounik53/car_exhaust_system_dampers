const int relayOffPlus = 4; //Реле +
const int relayOnPlus = 5;  //Реле +
const int relayMinus = 6;   //Реле - По умолчанию минус на плюсе закрывающего положения
const int relayDiod = 8;    //Реле управления светодиодом
const int buttonPin = 2;    //Кнопка

bool flag_button = false;   //Положение кнопки, для отслеживания однократного нажатия
bool flag_state = false;    //Текущее положение заслонки 
uint32_t btnTimer = 0;      //Таймер для убирания дребезга

//Конфигурация
bool debug = false;         //Флаг для включения отладки в порт 
bool startup_sound = false; //Флаг для определения запуска на открытых или закрытых заслонках (false - закрытые)

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
    //Запуск на громкой машине
    digitalWrite(relayMinus, LOW);
    delay(10);
    digitalWrite(relayOffPlus, LOW);
    delay(2000);
    digitalWrite(relayOffPlus, HIGH);
    digitalWrite(relayDiod, LOW);
    flag_state = !flag_state;
  } else {
    //запуск на тихой машине
    digitalWrite(relayOffPlus, LOW);
    delay(2000);
    digitalWrite(relayOffPlus, HIGH);
  }
}

void printToSerial(String message) {
  if (debug) {
    Serial.println(message);
  }
}

void change_state_relay(bool state) {
  if (state) {
    digitalWrite(relayMinus, LOW);
      delay(10);
      digitalWrite(relayOnPlus, LOW);
      printToSerial("Relay on up");
      delay(2000);
      digitalWrite(relayOnPlus, HIGH);
      digitalWrite(relayDiod, LOW);
      printToSerial("Vihlop open");
  } else {
      digitalWrite(relayMinus, HIGH);
      digitalWrite(relayOffPlus, LOW);
      printToSerial("Relay off up");
      delay(2000);
      digitalWrite(relayOffPlus, HIGH);
      digitalWrite(relayDiod, HIGH);
      printToSerial("Vihlop close");
  }
}

void loop() {
  bool btnState = digitalRead(buttonPin);
  if (btnState && !flag_button && millis() - btnTimer > 100) {
    flag_button = true;
    btnTimer = millis();
    printToSerial("button pressed");
    //Управление реле
    if (!flag_state) {
      change_state_relay(true);
      flag_state = !flag_state;
    } else {
      change_state_relay(false);
      flag_state = !flag_state;
    }
  }
  if (!btnState && flag_button && millis() - btnTimer > 100) {
    flag_button = false;
    btnTimer = millis();
  }
}