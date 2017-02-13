#include "TEE_UC20.h"
#include "SoftwareSerial.h"
#include <AltSoftSerial.h>
#include "call.h"
#include "sms.h"

CALL call;
SMS sms;
String phone_number = "0xxxxxxxxx";
//SoftwareSerial mySerial(8, 9); // RX, TX
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
  Serial.println("Send SMS");
  sms.DefaultSetting();
  sms.Start(phone_number);  
  sms.Sendln("hello");
  sms.Sendln("สวัสดี");
  sms.Sendln("วันนี้ท้องฟ้าแจ่มใส่");
  sms.Sendln("อากาศก็ดี้...ดี");
  sms.Stop();
  Serial.println("END"); 
 
}

void loop() 
{
 if (gsm.available())
  {
    Serial.write(gsm.read());
  } 
  if (Serial.available())
  {
    char c = Serial.read();
    gsm.write(c);
    
  }
}
