

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>

// Replace with your network credentials
const char* ssid = "TP-Link_A4FA";
const char* password = "47862890";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create a WebSocket object

AsyncWebSocket ws("/ws");
// Set LED GPIO
const int pinOut = 12;

String message = "";
String sliderValue1 = "0";
String sliderValue2 = "0";
String sliderValue3 = "0";
String sliderValue4 = "0";
uint32_t dutyCycle1;
int dutyCycle2;
uint32_t dutyCycle3;
int dutyCycle4;
int k = 1000000; //коэфф.перевода секунд в микросекунды

//Json Variable to Hold Slider Values
JSONVar sliderValues;

//Get Slider Values
String getSliderValues(){                                      //создает строку JSON с текущими значенимим сладеров
  sliderValues["sliderValue1"] = String(sliderValue1);
  sliderValues["sliderValue2"] = String(sliderValue2);
  sliderValues["sliderValue3"] = String(sliderValue3);
  sliderValues["sliderValue4"] = String(sliderValue4);
  String jsonString = JSON.stringify(sliderValues);
  return jsonString;
}

// Initialize SPIFFS
void initFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else{
   Serial.println("SPIFFS mounted successfully");
  }
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void notifyClients(String sliderValues) {                                   //уведомляет всех клиентов о текущих значениях ползунков
  ws.textAll(sliderValues);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {         //сервер прослушивает и обрабатывает то что пришло от клиента по протоколу вэбсокет 
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;
    if (message.indexOf("1s") >= 0) {
      sliderValue1 = message.substring(2);
      Serial.print("{sliderValue1 : ");Serial.print(sliderValue1); Serial.print("} ");
      
      //перевести sliderValue1 из String в Unsigned long
      //String переводим в массив
      char char_arr1[sliderValue1.length()+1];
      sliderValue1.toCharArray(char_arr1, sliderValue1.length()+1);        
      Serial.print("{char_arr1 : ");Serial.print(char_arr1);Serial.print("} "); 
      // массив arr переводим в Unsigned long
      dutyCycle1 = strtoul(char_arr1, NULL, 10);
      //dutyCycle1 = map(dutyCycle1, 0, 240000, 0, 240000);
      Serial.print("{dutyCycle1 : ");
      Serial.print(dutyCycle1); Serial.print("} ");
      Serial.println(getSliderValues());
      notifyClients(getSliderValues());
    }
    if (message.indexOf("2s") >= 0) {
      sliderValue2 = message.substring(2);
      Serial.print("{sliderValue2 : ");
      Serial.print(sliderValue2 ); Serial.print("} ");
      dutyCycle2 = map(sliderValue2.toInt(), 0, 100, 0, 100);
      Serial.print("{dutyCycle2 : ");
      Serial.print(dutyCycle2); Serial.print("} ");
      Serial.println(getSliderValues());
      notifyClients(getSliderValues());
    }    
    if (message.indexOf("3s") >= 0) {
      sliderValue3 = message.substring(2);
      Serial.print("(sliderValue3 : ");
      Serial.print(sliderValue3); Serial.print("} ");
      //необходимо перевести sliderValue3 из String в Unsigned long
      //String переводим в массив
      char char_arr3[sliderValue3.length()+1];
      sliderValue3.toCharArray(char_arr3, sliderValue3.length()+1);        
      Serial.print("{char_arr3 : ");Serial.print(char_arr3);Serial.print("} "); 
      // массив arr переводим в Unsigned long
      dutyCycle3 = strtoul(char_arr3, NULL, 10);
      //dutyCycle3 = map(sliderValue3.toInt(), 0, 240000, 0, 240000);
      Serial.print("{dutyCycle3 : ");
      Serial.print(dutyCycle3); Serial.print("} ");
      Serial.println(getSliderValues());
      notifyClients(getSliderValues());
    }
    if (message.indexOf("4s") >= 0) {
      sliderValue4 = message.substring(2);
      Serial.print("{sliderValue4 : ");
      Serial.print(sliderValue4 ); Serial.print(" } ");
      dutyCycle4 = map(sliderValue4.toInt(), 0, 100, 0, 100);
      Serial.print("dutyCycle4 : ");
      Serial.print(dutyCycle4); Serial.print("} ");
      Serial.println(getSliderValues());
      notifyClients(getSliderValues());
    }    
    if (strcmp((char*)data, "getValues") == 0) {                  //когда сервер получает getValues сообщение, он отправляет текущие значения ползунка
      notifyClients(getSliderValues());
    }
  }
}


void genSignal() {
  uint32_t i; 
  unsigned long  h_Freq = dutyCycle1;
  unsigned long  h_Duty = dutyCycle2;
  unsigned long  l_Freq = dutyCycle3;
  unsigned long  l_Duty = dutyCycle4;
  unsigned long h_Total = 0;
  unsigned long h_Ton = 0;
  unsigned long h_Tof = 0;
  unsigned long l_Total = 0;
  unsigned long l_Ton = 0;
  unsigned long l_Tof= 0;
  

  // для высокой частоты
  if (h_Freq !=0) {
    h_Total = k / h_Freq;
    h_Ton = h_Duty * h_Total / 100;
    h_Tof = h_Total - h_Ton;
  }
  else {
    h_Total = k / 1;
    h_Ton = h_Duty * h_Total / 100;
    h_Tof = h_Total - h_Ton;
  }

  // для низкой частоты
  if (l_Freq !=0) {
    l_Total = k / l_Freq;
    l_Ton = l_Duty * l_Total / 100;
    l_Tof = l_Total - l_Ton; 
  }
  else {
    l_Total = k / 1;
    l_Ton = l_Duty * l_Total / 100;
    l_Tof = l_Total - l_Ton; 
  }
  
  for (i = 1; i * h_Total < l_Ton; i++) {    //сигнал l_Ton
    digitalWrite(pinOut, HIGH);              //состоящий из пачки 
    delayMicroseconds(h_Ton);                //сигналов h_Ton
    digitalWrite(pinOut, LOW);
    delayMicroseconds(h_Tof);       
  }
  digitalWrite(pinOut, LOW);                 //сигнал l_Tof
  delayMicroseconds(l_Tof);

}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}


void setup() {
  Serial.begin(115200);
  pinMode(pinOut, OUTPUT);
  initFS();
  initWiFi();
  initWebSocket();
  
  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  server.serveStatic("/", SPIFFS, "/");

  // Start server
  server.begin();

}

void loop() {
  genSignal();
  ws.cleanupClients();
}
