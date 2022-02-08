// (с) Школа 1103 Москва Проект умный дом. Код для Arduino Uno
#include <MsTimer2.h> // pwm 3 11
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Servo.h> // pwm 9 10 
#define PIN_DHT 2 // датчик температуры
#define PIN_BTN 4 // кнопка
#define PIN_LEDR 5 // rgb светодиод
#define PIN_LEDG 6 // rgb светодиод
#define PIN_LEDB 10 // rgb светодиод
#define PIN_LED_1 7 // // красный светодиод
#define PIN_LED_2 8 // синий светодиод
#define PIN_SERVO 9 // сервопривод
#define PIN_KNOCK 12 // датчик удара
#define PIN_LED_3 13 // индикаторный светодиод
#define PIN_DAC A0 // фоторезистор
#define PIN_LED_4 A5 // светодиод пароля
#define TIMER_PERIOD 10 // период аппаратного таймера
#define CYCLE_1_TIME 100    // время цикла 1 ( TIMER_PERIOD*100=1000 мс)
#define CYCLE_2_TIME 200  // время цикла 2 (  TIMER_PERIOD*200=2000 мс)
#define CYCLE_3_TIME 1500  // время цикла 3 (  TIMER_PERIOD*1500= 15 с)
#define BOUNCE_DELAY 50 // время на дребезг кнопки в мс
#define KNOCK_DELAY 40 // время на дребезг датчика удара в мс
#define SHORT_PRESS 400 // время короткого нажатия в мс
#define PASS_LEN 4 // длина пароля
#define WATCH_DOOR 6 // время в секундах для ввода пароля
#define WATCH_SERVO 3 // время в секундах для сервопривода
#define WATCH_LED_4 4 // время в секундах для светодиода пароля
#define LOW_LIGHT 85 // уставка света минимум
#define HI_LIGHT 200 // уставка света минимум
#define LOW_TEMP 22 // уставка температуры на включение обогрева
#define HI_TEMP 27 // уставка температуры на включение охлаждения
#define ALARM_TEMP 28 // уставка температуры на сигнализацию
#define SERVO_CLOSE 2000 // угол закрытия
#define SERVO_OPEN 544 // угол открытия
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
 byte cnt=0; // счетчик для работы с паролем
 byte cnt_watch_door=0; // счетчик таймера для ввода пароля кнопки
 byte cnt_watch_door2=0; // счетчик таймера для ввода пароля датчик удара
 byte cnt_servo=0; // счетчик таймера для сервопривода
 byte cnt_pass_led=0; // счетчик таймера для светодиода пароля
 boolean startCode = false; // флаг начала кодовой последовательности кнопки
 byte led_mode =1; // режим работы светодиода 1 - работа 2 - прием пароля 3
 boolean blinker=false; // мигание светодиода 3
 boolean blinker_led4=false; // мигание светодиода 4
 boolean door_open=false; // открытие двери
 boolean knockState=false; // состояние датчика удара
 boolean flagKnockPressed=false; // флаг срабатывания датчика удара
 unsigned long knockTimer = 0; // таймер для устранения дребезга датчика удара
 boolean startCode2=false; // флаг начала кодовой последовательности датчика удара
 byte cnt2=0; // счетчик для работы с паролем
 
boolean password[PASS_LEN] ={ true,  true, false, false  }; // ключ открытия двери false - короткое true - динное нажатие
boolean key[PASS_LEN] = { false,  false, false, false  }; // текущий прием пароля
boolean password_good =false; //совпадение пароля по кнопке
boolean password_good2 =false; //совпадение пароля по датчику удара
unsigned long pulse_widht = 0; // длительность нажатия


//структура для значений исполнителей 
struct myDrive {
  byte LedR;
  byte LedG;
  byte LedB;
  boolean LedRed;
  boolean LedBlue;
  boolean Auto;
  };

//структура для значений датчиков 
struct mySensors {
  byte Temp;
  byte Hum;
  byte Light;
  boolean TempAlarm;
  };

myDrive Drive; // переменная под структуру
mySensors Sensors; // переменная под структуру
DHT_Unified dht(PIN_DHT, DHT11); // датчик температуры и влажности
Servo door_servo; // сервопривод
void setup() {
  
Serial.begin(115200);
Serial.setTimeout(2); // таймаут 2 мс
pinMode(PIN_LED_1, OUTPUT);
pinMode(PIN_LED_2, OUTPUT);
pinMode(PIN_LED_3, OUTPUT);
pinMode(PIN_LEDR, OUTPUT);
pinMode(PIN_LEDG, OUTPUT);
pinMode(PIN_LEDB, OUTPUT);
pinMode(PIN_KNOCK, INPUT);
pinMode(PIN_BTN, INPUT_PULLUP);
pinMode(PIN_LED_4, OUTPUT);
dht.begin(); // инициализация датчика температуры и влажности
digitalWrite(PIN_LED_4,LOW);
door_servo.attach(PIN_SERVO); // инициализация сервопривода
door_servo.writeMicroseconds(SERVO_CLOSE); // угол закрытия двери
delay(1500);
door_servo.detach(); //  освободили пин сервопривода для pwm rgb светодиода
// инициализация структур
Drive.LedR=0;
Drive.LedB=0;
Drive.LedG=0;
Drive.LedRed=false;
Drive.LedBlue=false;
Drive.Auto=true;

Sensors.Temp=0;
Sensors.Hum=0;
Sensors.Light=0;
Sensors.TempAlarm=false;

MsTimer2::set(TIMER_PERIOD, timerInterupt); // установка аппаратного таймера
MsTimer2::start(); // пуск аппаратного таймера
}

void loop() {

   
// правильный пароль замка
 if (password_good == true && password_good2==true){
  password_good = false ; // сбросили
  password_good2 = false ; // сбросили
  // открыли замок
  door_servo.attach(PIN_SERVO); // подсоединили сервопривод
  door_servo.writeMicroseconds(SERVO_OPEN);
  door_open = true; // дверь открыта
  timerCount3=0; // запустили таймер на 15 с
//  Serial.print("servo160 door_open=");Serial.println(door_open);
  
 }

  
//*************************программные таймеры****************************
//**************Таймер 1********************** 
  if ( flagTimer1 == true ) {
    flagTimer1= false;
    // ТАЙМЕР 1

 // читаем  методом readBytes() если есть данные
      if (Serial.readBytes((byte*)&Drive, sizeof(Drive))) {
     // получили данные от ESP из облака, обновляем исполнительные механизмы
     
     if  (Drive.Auto == false){ //управляем климатом из облака
     digitalWrite(PIN_LED_1,Drive.LedRed);
     digitalWrite(PIN_LED_2,Drive.LedBlue); 
     }
     if (Drive.Auto == false) { // управляем светом из облака
      analogWrite(PIN_LEDR, Drive.LedR);
      analogWrite(PIN_LEDG, Drive.LedG);
      analogWrite(PIN_LEDB, Drive.LedB);
     }
    }
    // режимы работы светодиода PIN_LED_3
    switch (led_mode) { 
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
// автоматическое управление светом
if (Drive.Auto == true) {
         if  (Sensors.Light<LOW_LIGHT) { // включаем свет на всю
          analogWrite(PIN_LEDR, 255);
          analogWrite(PIN_LEDG, 255);
          analogWrite(PIN_LEDB, 255);
        }
        if  (Sensors.Light>LOW_LIGHT && Sensors.Light<HI_LIGHT) { // включаем свет на серединку
          analogWrite(PIN_LEDR, 127);
          analogWrite(PIN_LEDG, 10);
          analogWrite(PIN_LEDB, 10);
        }
         if  (Sensors.Light>HI_LIGHT) { // выключаем свет 
          analogWrite(PIN_LEDR, 0);
          analogWrite(PIN_LEDG, 0);
          analogWrite(PIN_LEDB, 0);
        }
     }

 // автоматическое управление микроклиматом
    if  (Drive.Auto == true) {
    
       if (Sensors.Temp<LOW_TEMP) {
       digitalWrite(PIN_LED_1,HIGH); // если температура меньше уставки включить обогрев 
       }
       else {
       digitalWrite(PIN_LED_1,LOW); // если температура больше уставки выключить обогрев
       }
       if (Sensors.Temp>HI_TEMP) {
       digitalWrite(PIN_LED_2,HIGH); // если температура больше  уставки включить охлаждение 
       }
       else {
       digitalWrite(PIN_LED_2,LOW); // если температура меньше уставки выключить охлаждение
       }
      }
  // повышение температуры
  if (Sensors.Temp>ALARM_TEMP) {
       Sensors.TempAlarm=true; // если температура выше уставки включить сигнализацию 
       }
      else {
      Sensors.TempAlarm=false; // если температура ниже уставки выключить сигнализацию
    }

 // таймер ввода пароля от кнопки
   if (cnt>0) {
    cnt_watch_door++;
   }
   if (cnt_watch_door>WATCH_DOOR){
    // не успели ввести пароль, сброс
    cnt=0;
    led_mode=1;
    cnt_watch_door=0;
    password_good=false;
   }
 // таймер ввода пароля от датчика удара
    if (cnt2>0) {
    cnt_watch_door2++;
    
   }
   if (cnt_watch_door2>WATCH_DOOR){
    // не успели ввести пароль, сброс
    cnt2=0;
    led_mode=1;
    cnt_watch_door2=0;
    password_good2=false;
   }
   // таймер сервопривода
   if (cnt_servo>0){
   cnt_servo++; 
  // Serial.print("servo counter=");  Serial.println(cnt_servo);
   }
   if (cnt_servo>WATCH_SERVO){
   door_servo.detach(); //  освободили пин сервопривода для pwm rgb светодиода
//   Serial.println("servo free");
   cnt_servo=0; // остановили таймер
   } 

// таймер светодиода  led_4 ввода пароля
    if (cnt_pass_led>0) {
   cnt_pass_led++;
   blinker_led4=!blinker_led4;
   digitalWrite(PIN_LED_4,blinker_led4);
   }
  if (cnt_pass_led>WATCH_LED_4) {
    blinker_led4=false;
    digitalWrite(PIN_LED_4,blinker_led4);
    cnt_pass_led=0; // остановили мигание led_4
  }
  }
  
//**************Таймер 2********************** 
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

//**************Таймер 3********************** 
if ( flagTimer3 == true ) {
    flagTimer3= false;
    // ТАЙМЕР 3  

 if (door_open == true) { // если дверь открыта
 // Serial.print("servo 0 door_open=");Serial.println(door_open);
  door_open = false;
  door_servo.writeMicroseconds(SERVO_CLOSE); // закрыли дверь через 15 с
  cnt_servo=1;
  
 }
  
  }

//********датчик удара*********
    knockState = !digitalRead(PIN_KNOCK); // считали датчик удара инвертировано для удобства
    if (knockState && !flagKnockPressed && millis() - knockTimer > KNOCK_DELAY) {
    flagKnockPressed = true; // срабатывание датчика
    //Serial.print ("pressed ");
    if (cnt2 > 0){
      pulse_widht=millis()-knockTimer; // длина замера
    //   Serial.println(pulse_widht);
     }
      knockTimer = millis();
    }
  //*********отпускание 
  if (!knockState && flagKnockPressed && millis() - knockTimer > KNOCK_DELAY) {
    flagKnockPressed = false; 
    startCode2 = true;
    knockTimer = millis();
   // Serial.print("unpressed "); Serial.println(knockTimer);
   }

//****************кодовый замок*************
// по датчику удара
if (startCode2 == true) { // если кнопка отпущена, был замер
   startCode2 = false;
   led_mode = 2; // режим работы светодиода 2
 //  Serial.print("cnt2="); Serial.println(cnt2);
      if (cnt2>0 && cnt2<=PASS_LEN) {
      key[cnt2-1]=pulse_widht>SHORT_PRESS; // записываем в массив текущее нажатие  false - короткое true - динное нажатие
 //     Serial.print(cnt2-1); Serial.print(" ");Serial.println(pulse_widht);
      }
    cnt2++;
    if (cnt2>PASS_LEN) { // получили все нажатия по длине пароля
      for (byte i=0; i < PASS_LEN; i++){
        if (key[i] == password [i]) { // проверяем правильность пароля
          password_good2 =true;
    //      Serial.println("pass good");
          }
        else {
            password_good2 =false;
    //        Serial.println("pass wrong");
            break;
          }
          }
   
     cnt2=0; // готовы к новой проверке пароля
     led_mode = 1; // режим работы светодиода 1
     if (password_good2=true) {
      cnt_pass_led=1; // включили индикаторный светодиод
     }
     //Serial.print("pass2= "); Serial.println(password_good2);
    }
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
 //   Serial.print ("pressed ");
     if (cnt > 0){
      pulse_widht=millis()-btnTimer; // длина замера
 //      Serial.println(pulse_widht);
     }
    btnTimer = millis();
    }
  //*********отпускание кнопки
  if (!btnState && flagBtnPressed && millis() - btnTimer > BOUNCE_DELAY) {
    flagBtnPressed = false; // кнопка отжата
    startCode =true;
 //   Serial.println("unpressed ");
    btnTimer = millis();
   }

//****************кодовый замок*************
// по кнопке
if (startCode == true) { // если кнопка отпущена, был замер
   startCode = false;
   led_mode = 2; // режим работы светодиода 2
//   Serial.print("cnt="); Serial.println(cnt);
      if (cnt>0 && cnt<=PASS_LEN) {
      key[cnt-1]=pulse_widht>SHORT_PRESS; // записываем в массив текущее нажатие  false - короткое true - динное нажатие
 //     Serial.print(cnt-1); Serial.print(" ");Serial.println(pulse_widht);
      }
    cnt++;
    if (cnt>PASS_LEN) { // получили все нажатия по длине пароля
      for (byte i=0; i < PASS_LEN; i++){
        if (key[i] == password [i]) { // проверяем правильность пароля
          password_good =true;
   //       Serial.println("pass good");
          }
        else {
            password_good =false;
  //          Serial.println("pass wrong");
            break;
          }
          }
   
     cnt=0; // готовы к новой проверке пароля
     led_mode = 1; // режим работы светодиода 1
     if (password_good=true) {
      cnt_pass_led=1; // включили индикаторный светодиод
     }
   //  Serial.print("pass1= "); Serial.println(password_good);
    }
 }



 
  }
