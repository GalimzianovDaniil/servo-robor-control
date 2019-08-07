

#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>  
#include <FS.h>

Servo servo0;
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

const char* ssid = "www";
const char* password = "qwerty12";

ESP8266WebServer HTTP(80);

File fsUploadFile;

#define servoPin0 D1
#define servoPin1 D2
#define servoPin2 D3
#define servoPin3 D4
#define servoPin4 D5

void SSDP_init(void) {                                                  // Функция формирующая описательные свойства устройства для его обнаружения в сетевом окружении
  HTTP.on("/description.xml", HTTP_GET, []() {                          // Если происходит обращение к файлу /description.xml в корне устройства
    SSDP.schema(HTTP.client());                                         // Создаем SSDP-схему описания устройства
  });
  SSDP.setDeviceType("upnp:rootdevice");                                // Тип устройства 
  SSDP.setSchemaURL("description.xml");                                 // Файл с описанием
  SSDP.setHTTPPort(80);                                                 // Порт работы с HTTP
  SSDP.setName("Robot");                                              // Нзвание устройства в сетевом окружении
  SSDP.setSerialNumber(ESP.getChipId());                                // Серийный номер (для заполнения используем ID чипа ESP)
  SSDP.setURL("/");                                                     // Адрес по которому производится обращение к устройству при инициализации подключения
  SSDP.setModelName("Robot");                                         // Название модели устройства
  SSDP.setModelNumber("000000000001");                                  // Номер модели
  SSDP.setManufacturer("Galimzyanov Daniil");                                       // Производитель
  SSDP.setManufacturerURL("https://vk.com/danya_galimzyanov");                          // Адрес сайта производителя
  SSDP.begin();                                                         // Отдаем сформированную SSDP-схему описания устройства
}
String getContentType(String filename){                                 // Функция, возвращающая необходимый заголовок типа содержимого в зависимости от расширения файла
  if (HTTP.hasArg("download")) return "application/octet-stream";       // Если HTTP имеет аргументом "download", то отдаем содержимое файла для скачивания и завершаем выполнение функции 
  else if (filename.endsWith(".htm")) return "text/html";               // Если файл заканчивается на ".htm", то возвращаем заголовок "text/html" и завершаем выполнение функции
  else if (filename.endsWith(".html")) return "text/html";              // Если файл заканчивается на ".html", то возвращаем заголовок "text/html" и завершаем выполнение функции
  else if (filename.endsWith(".css")) return "text/css";                // Если файл заканчивается на ".css", то возвращаем заголовок "text/css" и завершаем выполнение функции
  else if (filename.endsWith(".js")) return "application/javascript";   // Если файл заканчивается на ".js", то возвращаем заголовок "application/javascript" и завершаем выполнение функции
  else if (filename.endsWith(".png")) return "image/png";               // Если файл заканчивается на ".png", то возвращаем заголовок "image/png" и завершаем выполнение функции
  else if (filename.endsWith(".gif")) return "image/gif";               // Если файл заканчивается на ".gif", то возвращаем заголовок "image/gif" и завершаем выполнение функции
  else if (filename.endsWith(".jpg")) return "image/jpeg";              // Если файл заканчивается на ".jpg", то возвращаем заголовок "image/jpeg" и завершаем выполнение функции
  else if (filename.endsWith(".ico")) return "image/x-icon";            // Если файл заканчивается на ".ico", то возвращаем заголовок "image/x-icon" и завершаем выполнение функции
  else if (filename.endsWith(".xml")) return "text/xml";                // Если файл заканчивается на ".xml", то возвращаем заголовок "text/xml" и завершаем выполнение функции
  else if (filename.endsWith(".pdf")) return "application/x-pdf";       // Если файл заканчивается на ".pdf", то возвращаем заголовок "application/x-pdf" и завершаем выполнение функции
  else if (filename.endsWith(".zip")) return "application/x-zip";       // Если файл заканчивается на ".zip", то возвращаем заголовок "application/x-zip" и завершаем выполнение функции
  else if (filename.endsWith(".gz")) return "application/x-gzip";       // Если файл заканчивается на ".gz", то возвращаем заголовок "application/x-gzip" и завершаем выполнение функции
  return "text/plain";                                                  // Если ни один из типов файла не совпал, то считаем что содержимое файла текстовое, отдаем соответствующий заголовок и завершаем выполнение функции
}
bool handleFileRead(String path){                       // Функция работы с файловой системой
  if(path.endsWith("/")) path += "index.html";           // Если устройство вызывается по корневому адресу, то должен вызываться файл index.htm (добавляем его в конец адреса)
  String contentType = getContentType(path);            // С помощью функции getContentType (описана ниже) определяем по типу файла (в адресе обращения) какой заголовок необходимо возвращать по его вызову
  String pathWithGz = path + ".gz";                     // Заводим еще один путь с адресом заархивированной версии файла из адреса обращения
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){ // Если в файловой системе существует заархивированный или простой файл по адресу обращения
    if(SPIFFS.exists(pathWithGz))                       //  Если существует заархивированный файл,
      path += ".gz";                                    //    то добавляем в адрес обращения окончание, указывающее на архив
    File file = SPIFFS.open(path, "r");                 //  Открываем файл для чтения
    size_t sent = HTTP.streamFile(file, contentType);   //  Выводим содержимое файла по HTTP, указывая заголовок типа содержимого contentType
    file.close();                                       //  Закрываем файл
    return true;                                        //  Завершаем выполнение функции, возвращая результатом ее исполнения true (истина)
  }
  return false;                                         // Завершаем выполнение функции, возвращая результатом ее исполнения false (если не обработалось предыдущее условие)
}



void servoInit() {
  servo0.attach(servoPin0);
  servo1.attach(servoPin1); 
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);
  Serial.println("servo Init done!");
}

void servoDetect() {
 int servoNum = HTTP.arg("servo").toInt();
 int servoAngle = HTTP.arg("angle").toInt();
 Serial.println(servoNum);
 Serial.println(servoAngle);
 digitalWrite(D7, HIGH);
 if (servoNum == 0) servo0.write(servoAngle);    
 if (servoNum == 1) servo1.write(servoAngle);
 if (servoNum == 2) servo2.write(servoAngle);
 if (servoNum == 3) servo3.write(servoAngle);
 if (servoNum == 4) servo4.write(servoAngle);
 HTTP.send(200, "text/plain", "OK");
 }
void script0() {
  servo0.write(150);
  delay(1000);
  servo0.write(0);
  delay(1000);
  servo1.write(150);
  delay(1000);
  servo1.write(0);
  
}
void scriptDetect() {
 int scriptNum = HTTP.arg("script-num").toInt();
 if (scriptNum == 0) script0();
 HTTP.send(200, "text/plain", "OK");
}


void pinsInit() {
  pinMode(D7, OUTPUT);
  pinMode(servoPin0, OUTPUT);
  pinMode(servoPin1, OUTPUT);
}
void setup() {
pinsInit();
servoInit();
SSDP_init();
SPIFFS.begin();
HTTP.begin();
servo0.write(90);
servo0.write(0);
Serial.begin(115200);
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
Serial.println(WiFi.localIP());

HTTP.on("/servo", servoDetect);
HTTP.on("/script", scriptDetect);
HTTP.onNotFound([](){                                   // Описываем действия при событии "Не найдено"
    if(!handleFileRead(HTTP.uri()))                       // Если функция handleFileRead (описана ниже) возвращает значение false в ответ на поиск файла в файловой системе
      HTTP.send(404, "text/plain", "-----");   
  });
}

void loop() {
  HTTP.handleClient();
  delay(1);
}
