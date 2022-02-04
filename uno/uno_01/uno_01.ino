// (с) Школа 1103 Москва Проект умный дом. Код для Arduino Uno
#include <MsTimer2.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define PIN_DHT 2 // датчик температуры
#define PIN_BTN 4 // кнопка
#define PIN_LEDR 5 // rgb светодиод
#define PIN_LEDG 6 // rgb светодиод
#define PIN_LEDB 9 // rgb светодиод
#define PIN_LED_1 7 // // красный светодиод
#define PIN_LED_2 8 // синий светодиод
#define PIN_LED_3 13 // зеленый светодиод
#define PIN_DAC A0 // фоторезистор
#define TIMER_PERIOD 10 // период аппаратного таймера
#define CYCLE_1_TIME 100    // время цикла 1 ( TIMER_PERIOD*100=1000 мс)
#define CYCLE_2_TIME 200  // время цикла 2 (  TIMER_PERIOD*200=2000 мс)
#define CYCLE_3_TIME 1500  // время цикла 3 (  TIMER_PERIOD*1500= 15 с)
#define BOUNCE_DELAY 50 // время на дребезг кнопки в мс
#define SHORT_PRESS 350 // время короткого нажатия в мс
#define PASS_LEN 6 // длина пароля
//переменные
 byte  timerCount1=0;    // счетчик таймера 1
 byte  timerCount2=0;    // счетчик таймера 2
 unsigned int  timerCount3=0;    // счетчик таймера 3
 boolean flagTimer1=false;   // признак программного таймера 1
 boolean flagTimer2=false;   // признак программного таймера 2
 boolean flagTimer3=false;   // признак программного таймера 3
 boolean btnState=false; // состояние кнопки
 boolean flagBtnPressed=false; // флаг нажатия кнопки
 unsigned long btnTimer = 0; // таймер для устранения дребезга кнопки
 byte cnt=0; // счетчик
 boolean startCode = false; // флаг начала кодовой последовательности
 byte led_mode =1; // режим работы светодиода 1 - работа 2 - прием пароля 3
 boolean blinker=false; // мигание светодиода

boolean password[PASS_LEN] ={ true,  true, false, false ,false ,true }; // ключ открытия двери false - короткое true - динное нажатие
boolean key[PASS_LEN] = { false,  false, false, false ,false ,false }; // текущий прием пароля
boolean password_good =false; //совпадение пароля
unsigned long pulse_widht = 0; // длительность нажатия

//структура для значений исполнителей 
struct myDrive {
  byte LedR;
  byte LedG;
  byte LedB;
  boolean LedRed;
  boolean LedBlue;
};

//структура для значений датчиков 
struct mySensors {
  byte Temp;
  byte Hum;
  byte Light;
  };

myDrive Drive; // переменная под структуру
mySensors Sensors; // переменная под структуру
DHT_Unified dht(PIN_DHT, DHT11); // датчик температуры и влажности
void setup() {
  
Serial.begin(115200);
pinMode(PIN_LED_1, OUTPUT);
pinMode(PIN_LED_2, OUTPUT);
pinMode(PIN_LED_3, OUTPUT);
pinMode(PIN_LEDR, OUTPUT);
pinMode(PIN_LEDG, OUTPUT);
pinMode(PIN_LEDB, OUTPUT);
pinMode(PIN_BTN, INPUT);
dht.begin(); // инициализация датчика температуры и влажности

// инициализация структур
Drive.LedR=0;
Drive.LedB=0;
Drive.LedG=0;
Drive.LedRed=false;
Drive.LedBlue=false;
Sensors.Temp=0;
Sensors.Hum=0;
Sensors.Light=0;


MsTimer2::set(TIMER_PERIOD, timerInterupt); // установка аппаратного таймера
MsTimer2::start(); // пуск аппаратного таймера
}

void loop() {


// читаем  методом readBytes() если ессть данные
   if (Serial.readBytes((byte*)&Drive, sizeof(Drive))) {
     // получили данные от ESP из облака, обновляем исполнительные механизмы
     digitalWrite(PIN_LED_1,Drive.LedRed);
     digitalWrite(PIN_LED_2,Drive.LedBlue); 
     analogWrite(PIN_LEDR, Drive.LedR);
     analogWrite(PIN_LEDG, Drive.LedG);
     analogWrite(PIN_LEDB, Drive.LedB);
  }

// правильный пароль замка
 if (password_good == true){
  password_good = false ; // сбросили
  // открыли замок
  //digitalWrite(PIN_LED_3,HIGH); // включили светодиод пока
  timerCount3=0; // запустили таймер на 15 с
  
 }

  
//*************************программные таймеры****************************
  if ( flagTimer1 == true ) {
    flagTimer1= false;
    // ТАЙМЕР 1
     
    switch (led_mode) { // режимы работы светодиода PIN_LED_3
      case 1:
      // в работе
      blinker=!blinker;
      digitalWrite(PIN_LED_3,blinker);
      break;
      case 2:
      // замок
      digitalWrite(PIN_LED_3, LOW);
      break;
    }
     
   
   
  }

  if ( flagTimer2 == true ) {
    flagTimer2= false;
    // ТАЙМЕР 2   опрос датчиков
  sensors_event_t event;
  dht.temperature().getEvent(&event); // считать температуру
  if (isnan(event.temperature)) {
  Sensors.Temp = 0;}
   else {
    Sensors.Temp =(byte)event.temperature;
   }
  dht.humidity().getEvent(&event); // считать влажность
  
  if (isnan(event.relative_humidity)) {
  Sensors.Hum = 0; }
    else {
    Sensors.Hum = (byte)event.relative_humidity;  
    }
  
  Sensors.Light = analogRead(PIN_DAC) >>2; // считать датчик света

  Serial.write((byte*)&Sensors, sizeof(Sensors)); // отправили данные в ESP

  }

if ( flagTimer3 == true ) {
    flagTimer3= false;
    // ТАЙМЕР 3  


  // Drive.LedRed=!Drive.LedRed; 
 //  digitalWrite(PIN_LED_1,Drive.LedRed);
  }

 
}

void  timerInterupt(){
  //прерывание аппаратного таймера

timerCount1++;  // + 1 к счетчику таймера 1
timerCount2++;  // + 1 к счетчику таймера 2
timerCount3++;  // + 1 к счетчику таймера 3  

 if ( timerCount1 >= CYCLE_1_TIME ) {
    timerCount1= 0;     // сброс счетчика
    flagTimer1= true;   // установка флага таймера 1
  }

 if ( timerCount2 >= CYCLE_2_TIME ) {
    timerCount2= 0;     // сброс счетчика
    flagTimer2= true;   // установка флага таймера 2
  }

if ( timerCount3 >= CYCLE_3_TIME ) {
    timerCount3= 0;     // сброс счетчика
    flagTimer3= true;   // установка флага таймера 3
  }

  //********нажатие кнопки*********
  btnState = !digitalRead(PIN_BTN); // считали кнопку инвертировано для удобства
    if (btnState && !flagBtnPressed && millis() - btnTimer > BOUNCE_DELAY) {
    flagBtnPressed = true; // кнопка нажата
    btnTimer = millis();
    }
  //*********отпускание кнопки
  if (!btnState && flagBtnPressed && millis() - btnTimer > BOUNCE_DELAY) {
    flagBtnPressed = false; // кнопка отжата
    pulse_widht=millis()-btnTimer; // длина замера
    startCode =true;
    btnTimer = millis();
   }

//****************кодовый замок*************

if (startCode == true) { // если кнопка отпущена, был замер
   startCode = false;
   led_mode = 2; // режим работы светодиода 2
      if (cnt<PASS_LEN) {
      key[cnt]=pulse_widht>SHORT_PRESS; // записываем в массив текущее нажатие  false - короткое true - динное нажатие
      }
    cnt++;
    if (cnt>=PASS_LEN) { // получили все нажатия по длине пароля
      for (byte i=0; i < PASS_LEN; i++){
        if (key[i] == password [i]) { // проверяем правильность пароля
          password_good =true;
          }
        else {
            password_good =false;
          }
          }
   
     cnt=0; // готовы к новой проверке пароля
     led_mode = 1; // режим работы светодиода 1
    }
 }

 
  }
