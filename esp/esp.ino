// (с) Школа 1103 Москва Проект умный дом. Код для ESP8266

#include <iocontrol.h>
#include <ESP8266WiFi.h>
// Название панели на сайте iocontrol.ru
const char* myPanelName = "sch1103";
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
const char* VarName_Auto = "Auto";
const char* VarName_TempAlarm = "TempAlarm";

//const char* ssid = "AndroidAP3108";
//const char* password = "mifw4028";

const char* ssid = "GDR";
const char* password = "chika16!";

// Создаём объект клиента
WiFiClient client;

// Создаём объект iocontrol, передавая в конструктор название панели и клиента
iocontrol mypanel(myPanelName, client);


//структура для значений датчиков 
struct mySensors {
  byte Temp;
  byte Hum;
  byte Light;
  boolean TempAlarm; 
  };
//структура для значений исполнителей 
struct myDrive {
  byte LedR;
  byte LedG;
  byte LedB;
  boolean LedRed;
  boolean LedBlue;
  boolean Auto;
  };

myDrive Drive;
mySensors Sensors;
void setup() {
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
          
  }

  // ************************ ЗАПИСЬ ************************
    
    // Записываем  значение в переменную для отпраки в сервис
    mypanel.write(VarName_Temperature, Sensors.Temp);    
    mypanel.write(VarName_Humidity, Sensors.Hum);    
    mypanel.write(VarName_Light, Sensors.Light);    
    mypanel.write(VarName_TempAlarm, Sensors.TempAlarm);
    mypanel.write(VarName_Auto,Drive.Auto);
    // Отправляем переменные из контроллера в сервис
    status = mypanel.writeUpdate();
    // Если статус равен константе OK...
  /*  if (status == OK) {
        // Выводим текст в последовательный порт
  //      Serial.println("------- Write OK -------");
    }*/

    // ************************ ЧТЕНИЕ ************************

    // Чтение значений переменных из сервиса
    status = mypanel.readUpdate();
    // Если статус равен константе OK...
    if (status == OK) {
        
        // Записываем считанный из сервиса значения в переменные
        long io_Heat = mypanel.readInt(VarName_Heat);   
        long io_Cool = mypanel.readInt(VarName_Cool);   
        long io_LightRed = mypanel.readInt(VarName_LightRed);   
        long io_LightGreen = mypanel.readInt(VarName_LightGreen);   
        long io_LightBlue = mypanel.readInt(VarName_LightBlue);   
        long io_Auto = mypanel.readInt(VarName_Auto);   

Drive.LedR= (byte) io_LightRed;
Drive.LedB= (byte) io_LightBlue;
Drive.LedG= (byte) io_LightGreen;
Drive.LedRed= (boolean) io_Heat;
Drive.LedBlue=(boolean) io_Cool;   
Drive.Auto = (boolean) io_Auto;
    }

if ( Drive.LedRed == true ||  Drive.LedBlue == true || Drive.LedR>0   || Drive.LedB >0 || Drive.LedG >0) { // если есть команды из облака отключаем автоматическое управление
Drive.Auto = false; // пока не снимется на ноль всё управление режим авто не включится
}



Serial.write((byte*)&Drive, sizeof(Drive)); // отправили данные в Arduino
delay(1000); // обмен с панелью не чаще 1 раза в секуду
}
