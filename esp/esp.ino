// (с) Школа 1103 Москва Проект умный дом. Код для ESP8266

#include <iocontrol.h>
#include <ESP8266WiFi.h>
// Название панели на сайте iocontrol.ru
const char* myPanelName = "big2c";
int status;
// Название переменных как на сайте iocontrol.ru
const char* VarName_but1 = "but1";
const char* VarName_color = "color";
const char* VarName_Temperature = "Temperature";
const char* VarName_Humidity = "Humidity";
const char* VarName_Light = "Light";

const char* ssid = "R2D2";
const char* password = "rrDJLk6j*Pjb";

// Создаём объект клиента
WiFiClient client;

// Создаём объект iocontrol, передавая в конструктор название панели и клиента
iocontrol mypanel(myPanelName, client);


//структура для значений датчиков 
struct mySensors {
  byte Temp;
  byte Hum;
  int Light; 
  };
//структура для значений исполнителей 
struct myDrive {
  byte LedR;
  byte LedG;
  byte LedB;
  boolean LedRed;
  boolean LedBlue;
};

myDrive Drive;
mySensors Sensors;
void setup() {
Drive.LedR=0;
Drive.LedB=0;
Drive.LedG=0;
Drive.LedRed=false;
Drive.LedBlue=false;
Sensors.Temp=0;
Sensors.Hum=0;
Sensors.Light=0;
Serial.begin(115200);

WiFi.begin(ssid, password);

    // Ждём подключения
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    // Вызываем функцию первого запроса к сервису
    mypanel.begin();
}

void loop() {

  if (Serial.readBytes((byte*)&Sensors, sizeof(Sensors))) {
    Serial.print("T=");
    Serial.println(Sensors.Temp);
    Serial.print("H=");
    Serial.println(Sensors.Hum);
    Serial.print("L=");
    Serial.println(Sensors.Light);
        
  }

// ************************ ЧТЕНИЕ ************************

    // Чтение значений переменных из сервиса
    status = mypanel.readUpdate();
    // Если статус равен константе OK...
    if (status == OK) {
        // Выводим текст в последовательный порт
        Serial.println("------- Read OK -------");
        // Записываем считанный из сервиса значения в переменные
        
    } 

    // ************************ ЗАПИСЬ ************************
    
    // Записываем  значение в переменную для отпраки в сервис
    mypanel.write(VarName_Temperature, Sensors.Temp);    // Записали значение 12345
    mypanel.write(VarName_Humidity, Sensors.Hum);    // Записали значение 12345
    mypanel.write(VarName_Light, Sensors.Light);    // Записали значение 12345
    
    // Отправляем переменные из контроллера в сервис
    status = mypanel.writeUpdate();
    // Если статус равен константе OK...
    if (status == OK) {
        // Выводим текст в последовательный порт
        Serial.println("------- Write OK -------");
    }

}
