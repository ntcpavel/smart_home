// (с) Школа 1103 Москва Проект умный дом. Код для ESP8266

#include <iocontrol.h>
#include <ESP8266WiFi.h>
// Название панели на сайте iocontrol.ru
const char* myPanelName = "big2c";
int status;
// Название переменных как на сайте iocontrol.ru
const char* VarName_Temperature = "Temperature";
const char* VarName_Humidity = "Humidity";
const char* VarName_Light = "Light";
const char* VarName_Heat = "Heat";
const char* VarName_Cool = "Cool";
const char* VarName_LightRed = "LightRed";
const char* VarName_LightGreen = "LightGreen";
const char* VarName_LightBlue = "LightBlue";

const char* ssid = "AndroidAP3108";
const char* password = "mifw4028";

// Создаём объект клиента
WiFiClient client;

// Создаём объект iocontrol, передавая в конструктор название панели и клиента
iocontrol mypanel(myPanelName, client);


//структура для значений датчиков 
struct mySensors {
  byte Temp;
  byte Hum;
  byte Light; 
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
        //Serial.print(".");
        delay(500);
    }
    // Вызываем функцию первого запроса к сервису
    mypanel.begin();
}

void loop() {

  if (Serial.readBytes((byte*)&Sensors, sizeof(Sensors))) {
  /*  Serial.print("T=");
    Serial.println(Sensors.Temp);
    Serial.print("H=");
    Serial.println(Sensors.Hum);
    Serial.print("L=");
    Serial.println(Sensors.Light);*/
        
  }

    // ************************ ЧТЕНИЕ ************************

    // Чтение значений переменных из сервиса
    status = mypanel.readUpdate();
    // Если статус равен константе OK...
    if (status == OK) {
        // Выводим текст в последовательный порт
       // Serial.println("------- Read OK -------");
        // Записываем считанный из сервиса значения в переменные
        long io_Heat = mypanel.readInt(VarName_Heat);   // целочисленная переменна
       // Serial.println((String)"io_Heat = "+io_Heat);
        long io_Cool = mypanel.readInt(VarName_Cool);   // целочисленная переменна
      //  Serial.println((String)"io_Cool = "+io_Cool);
        long io_LightRed = mypanel.readInt(VarName_LightRed);   // целочисленная переменна
     //   Serial.println((String)"io_LightRed = "+io_LightRed);
        long io_LightGreen = mypanel.readInt(VarName_LightGreen);   // целочисленная переменна
    //    Serial.println((String)"io_LightGreen = "+io_LightGreen);
        long io_LightBlue = mypanel.readInt(VarName_LightBlue);   // целочисленная переменна
     //   Serial.println((String)"io_LightBlue = "+io_LightBlue);

Drive.LedR= (byte) io_LightRed;
Drive.LedB= (byte) io_LightBlue;
Drive.LedG= (byte) io_LightGreen;
Drive.LedRed= (boolean) io_Heat;
Drive.LedBlue=(boolean) io_Cool;    
//Serial.print("heat"); Serial.println(Drive.LedRed); 
//Serial.print("cool"); Serial.println(Drive.LedBlue); 

    }

    // ************************ ЗАПИСЬ ************************
    
    // Записываем  значение в переменную для отпраки в сервис
    mypanel.write(VarName_Temperature, Sensors.Temp);    
    mypanel.write(VarName_Humidity, Sensors.Hum);    
    mypanel.write(VarName_Light, Sensors.Light);    
    
    // Отправляем переменные из контроллера в сервис
    status = mypanel.writeUpdate();
    // Если статус равен константе OK...
    if (status == OK) {
        // Выводим текст в последовательный порт
  //      Serial.println("------- Write OK -------");
    }

Serial.write((byte*)&Drive, sizeof(Drive)); // отправили данные в Arduino
delay(500);
}
