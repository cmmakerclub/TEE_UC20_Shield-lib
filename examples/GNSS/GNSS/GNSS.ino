#include "TEE_UC20.h"
#include "SoftwareSerial.h"
#include <AltSoftSerial.h>
#include "gnss.h"
GNSS gps;

//SoftwareSerial mySerial(8, 9); // RX, TX
AltSoftSerial mySerial;
void debug(String data)
{
  Serial.println(data);
}
void data_out(char data)
{
  Serial.write(data);
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
  gps.Start();
  Serial.print(F("GPS Start"));
}

void loop() 
{
  Serial.println(gps.GetPosition());
  delay(500);   
}
