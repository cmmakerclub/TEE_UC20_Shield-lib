#include "TEE_UC20.h"
#include "SoftwareSerial.h"
#include <AltSoftSerial.h>
#include "call.h"
#include "sms.h"
CALL call;
SMS sms;
//SoftwareSerial mySerial(2, 3); // RX, TX
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
}

void loop() 
{
 
  if(gsm.event_input())
  {
    if(gsm.event_type == EVENT_RING)
    {
      Serial.println("Ring");
    }
    if(gsm.event_type == EVENT_SMS)
    {
      Serial.println("NEW SMS");
      Serial.print("index = ");
      int index = sms.IndexNewSMS();
      Serial.println(index);
      String str_sms =sms.ReadSMS(index);
      Serial.println(sms.ConvertStrUnicodeToTIS620(str_sms));
      sms.DeleteSMS(index);
    }
  }  
}
