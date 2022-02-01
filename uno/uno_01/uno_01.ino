#include <MsTimer2.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define STOP_TIME 15000 // время закрытия двери
#define PIN_DHT 2
#define PIN_LED_1 7
#define PIN_LED_2 8
#define TIMER_PERIOD 10 // период аппаратного таймера
#define CYCLE_1_TIME 50    // время цикла 1 ( TIMER_PERIOD*50=500 мс)
#define CYCLE_2_TIME 200  // время цикла 2 (  TIMER_PERIOD*200=2000 мс)
#define CYCLE_3_TIME 1500  // время цикла 3 (  TIMER_PERIOD*1500= 15 с)

// https://alexgyver.ru/lessons/parsing/ - стереть!

byte  timerCount1;    // счетчик таймера 1
byte  timerCount2;    // счетчик таймера 2
unsigned int  timerCount3;    // счетчик таймера 3
boolean flagTimer1;   // признак программного таймера 1
boolean flagTimer2;   // признак программного таймера 2
boolean flagTimer3;   // признак программного таймера 3

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
  unsigned int Light;
  };

myDrive Drive;
mySensors Sensors;
boolean Knock; // датчик удара
boolean Btn1; // кнопка
DHT_Unified dht(PIN_DHT, DHT11); // датчик температуры и влажности
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(PIN_LED_1, OUTPUT);
pinMode(PIN_LED_2, OUTPUT);
dht.begin(); // инициализация датчика температуры и влажности

// инициализация переменных
timerCount1=0;
timerCount2=0;
timerCount3=0;
flagTimer1=false;
flagTimer2=false;
flagTimer3=false;
Drive.LedR=0;
Drive.LedB=0;
Drive.LedG=0;
Drive.LedRed=false;
Drive.LedBlue=false;
Sensors.Temp=0;
Sensors.Hum=0;
Sensors.Light=0;
Knock = false;
Btn1 = false;

MsTimer2::set(TIMER_PERIOD, timerInterupt); // 10ms period timer
MsTimer2::start();
}

void loop() {


// читаем  методом readBytes()
  // указываем ему буфер-структуру, но приводим тип к byte*
  // размер можно указать через sizeof()
 /* if (Serial.readBytes((byte*)&buf, sizeof(buf))) {
    Serial.println(buf.val_b);
    Serial.println(buf.val_i);
    Serial.println(buf.val_l);
    Serial.println(buf.val_f); 
    //Serial.println(2456);
  }*/
  if ( flagTimer1 == true ) {
    flagTimer1= false;
    // ТАЙМЕР 1  
    Drive.LedBlue=!Drive.LedBlue;
    digitalWrite(PIN_LED_2,Drive.LedBlue); 
  }

  if ( flagTimer2 == true ) {
    flagTimer2= false;
    // ТАЙМЕР 2   
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  Serial.print(" %\t");
  Serial.print("Температура: ");
  Serial.print(event.temperature);
   //Sensors.Hum = (byte) dht.readHumidity(); //Измеряем влажность
  //Sensors.Temp = (byte) dht.readTemperature(); //Измеряем температуру
  dht.humidity().getEvent(&event);
  Serial.print("Влажность: ");
  Serial.print(event.relative_humidity);
  Serial.println(" *C "); //Вывод показателей на экран
  }

if ( flagTimer3 == true ) {
    flagTimer3= false;
    // ТАЙМЕР 3  


   Drive.LedRed=!Drive.LedRed; 
   digitalWrite(PIN_LED_1,Drive.LedRed);
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
  
  }
