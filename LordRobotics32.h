

#ifndef __LORDROBOTICS32_H
#define __LORDROBOTICS32_H




const uint8_t buzzer_ring_pin= 4;

void LR_InitBuzzer(void){
pinMode(buzzer_ring_pin,OUTPUT); 
}


void LR_BipBuzzer(uint8_t bip_time){
 for (int i =0;i<bip_time;i++){
 digitalWrite(buzzer_ring_pin,HIGH);
 delay(10);
 digitalWrite( buzzer_ring_pin,LOW);
 delay(10);
  } 
}











#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>


unsigned long currentMillis;
static unsigned long lastMsgUpdateTime = 0;



// WiFi Configuration
extern const char* ssid;
const char* password = ""; // Add password if needed


/* Web Server and WebSocket Configuration */
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

String ReceivedMsg = "";
bool   DeviceID = false;


/* Sensor Data Structure for send */
struct MsgPackegForSend {
  int HeartADCVal=0;
  float BpmVal=0;
  float Spo2Val=0;
  String ChartValName ="";
  String Msg="";
};

MsgPackegForSend msgPackageForSend;





void LR_ProcessReceivedCommand(){
      if (ReceivedMsg == "Test"){
      //LR_CarForward(CarSpeed,CarSpeed-5);       
      ReceivedMsg ="";
      //LR_BipBuzzer(5);
      msgPackageForSend.Msg= "Hello From Master";
  } 
}





/* WebSocket Event Handler*/
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED:
      Serial.printf("[%u] Connected!\n", num);
      break;
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_TEXT:
      // Handle incoming WebSocket messages
      ReceivedMsg = String((char*)payload);

      /* sadece istenilen Cihazdan gelen Komutları işlesin*/
      if(ReceivedMsg =="<...?..>"){ DeviceID = true;}
      //if(DeviceID){ 
        LR_ProcessReceivedCommand();
      //  }
      break;
  }
}





// HTTP Handlers
void HTTP_handleRoot() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200,"text/plain", "Lord Robotics Web Server");
}




void HTTP_handleState(){
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  
  // Create JSON response with car and sensor data
  DynamicJsonDocument doc(256);
  doc["RequestMsg"]  = "LWSA Firmware is active";
 
  String jsonResponse;
  serializeJson(doc,jsonResponse);
  server.send(200,"application/json", jsonResponse);
}





void LR_BroadcastMsg(){
    currentMillis = millis();

  // Her 500 ms de bir msaj paketini  yayınla
  if (currentMillis - lastMsgUpdateTime >= 500L) {
  
  DynamicJsonDocument doc(256);
   doc["Value1"]         = msgPackageForSend.BpmVal;
   doc["Value2"]         = msgPackageForSend.Spo2Val;
   doc["chartValue"]     = msgPackageForSend.HeartADCVal;
   doc["chartValueName"] = msgPackageForSend.ChartValName;
   doc["Msg"]            = msgPackageForSend.Msg;
  
  String jsonResponse;
  serializeJson(doc,jsonResponse);
  webSocket.broadcastTXT(jsonResponse);
 
  lastMsgUpdateTime = currentMillis;
  }
}




// WiFi and Server Initialization
void LR_InitWebServer() {
  // Configure WiFi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Configure Web Server
  server.on("/", HTTP_handleRoot);
  server.on("/state", HTTP_handleState);
  server.onNotFound(HTTP_handleRoot);
  server.begin();

  // Configure WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}








#endif
