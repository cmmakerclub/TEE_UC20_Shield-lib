#include "TEE_UC20.h"
#include "SoftwareSerial.h"
#include <AltSoftSerial.h>
#include "internet.h"
#include "uc_mqtt.h"
INTERNET net;
UCxMQTT mqtt;
//SIM TRUE  internet
#define APN "internet"
#define USER ""
#define PASS ""

#define MQTT_SERVER      "your mqtt server"
#define MQTT_PORT        "1883"
#define MQTT_ID          "your mqtt id"
#define MQTT_USER        "your user"
#define MQTT_PASSWORD    "your password"
#define MQTT_WILL_TOPIC    0
#define MQTT_WILL_QOS      0
#define MQTT_WILL_RETAIN   0
#define MQTT_WILL_MESSAGE  0


unsigned long previousmqtt = 0;
const long intervalmqtt = 1000;

AltSoftSerial mySerial;

void debug(String data)
{
  Serial.println(data);
}
void setup() 
{
  Serial.begin(9600);
  gsm.begin(&mySerial,9600);
  gsm.Event_debug = debug;
  Serial.println(F("UC20"));
  gsm.PowerOn(); 
  while(gsm.WaitReady()){}
  Serial.print(F("GetOperator --> "));
  Serial.println(gsm.GetOperator());
  Serial.print(F("SignalQuality --> "));
  Serial.println(gsm.SignalQuality());
  
  Serial.println(F("Disconnect net"));
  net.DisConnect();
  Serial.println(F("Set APN and Password"));
  net.Configure(APN,USER,PASS);
  Serial.println(F("Connect net"));
  net.Connect();
  Serial.println(F("Show My IP"));
  Serial.println(net.GetIP());
  mqtt.callback = callback;
  connect_server();
}

void callback(String topic ,char *playload,unsigned char length)
{
  Serial.println();
  Serial.println(F("%%%%%%%%%%%%%%%%%%%%%%%%%%%%"));
  Serial.print(F("Topic --> "));
  Serial.println(topic);
  playload[length]=0;
  String str_data(playload);
  Serial.print(F("Playload --> "));
  Serial.println(str_data);
}
void connect_server()
{
  do
  {
     Serial.println(F("Connect Server"));
     Serial.println(F("wait connect"));
      if(mqtt.DisconnectMQTTServer())
      {
        mqtt.ConnectMQTTServer(MQTT_SERVER,MQTT_PORT);
      }
      delay(500);
      Serial.println(mqtt.ConnectState());
  }
 while(!mqtt.ConnectState());
 Serial.println(F("Server Connected"));
 unsigned char ret = mqtt.Connect(MQTT_ID,MQTT_USER,MQTT_PASSWORD);
 Serial.println(mqtt.ConnectReturnCode(ret));
 mqtt.Publish("ctrl","hello world",false);
 mqtt.Subscribe("inTopic");
}
void loop() 
{
  unsigned long currentMillis = millis();
  if(currentMillis - previousmqtt >= intervalmqtt) 
  {
      mqtt.Publish("ctrl","Publish from 3G",false);
      previousmqtt = currentMillis; 
      if(mqtt.ConnectState()==false)
      {
        Serial.println(F("Reconnect"));
        connect_server();
      }
   }
   mqtt.MqttLoop();
}

