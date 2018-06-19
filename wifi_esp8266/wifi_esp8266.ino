#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
String json;
WiFiServer server(80);
IPAddress ip(192, 168, 11, 95); // where xx is the desired IP Address
IPAddress gateway(192, 168, 11, 1); // set gateway to match your network

void setup() {
  Serial.begin(115200); 
  IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
  WiFi.config(ip, gateway, subnet);
  Serial.println("Wifi config success");
}

void loop() {
  if(Serial.available()>0){
    json = Serial.readString();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    const String command = root["command"];
    String ip = root["ip"];
    int port = root["port"];
    String data = root["data"];

    Serial.println(command);
    Serial.println(ip);
    Serial.println(port);
    Serial.println(data);
    if(command == "SmartConfig")
    {
      SmartConfig();
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }
    else if(command == "Wifi_controlDevice"){
      Wifi_controlDevice(ip,port,data);
    }
  }

  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    while(!client.available()){
      delay(1);
    }
    // Read the first line of the request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();
    client.println("OK \n a");    
    delay(1);
    Serial.println("Client disconnected");
    Serial.println("");
  }
}

void SmartConfig(){
  Serial.println("State smartconfig");
  WiFi.disconnect();//clear all current wifi data 
  WiFi.mode(WIFI_STA);//set station mode or smart config
  
  delay(500);
  WiFi.beginSmartConfig();//begin smart config
  Serial.println("Smart config started");

  while(WiFi.status()!=WL_CONNECTED){// checking for wifi is connected or not
    Serial.print('.');
    delay(1000);
  }
  Serial.println("WIFI CONNECTED");
  Serial.print("IP :");
  Serial.print(WiFi.localIP());
  server.begin();
  Serial.println("Server started");
}

void Wifi_controlDevice(String ip,int port,String data){
  Serial.println("State control");
  WiFiClient client;
  if (client.connect(ip, port))
  {
    Serial.println("connected]");

    Serial.println("[Sending a request]");
    client.print(data);
    Serial.println("Response:");
    while (client.connected())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    client.stop();
    Serial.println("\n[Disconnected]");
  }
}
