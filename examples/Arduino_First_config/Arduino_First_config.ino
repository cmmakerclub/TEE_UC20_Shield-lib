#include <AltSoftSerial.h>
AltSoftSerial mySerial;
#define MAX_CMD 9
#define START_PIN 4
String CMD[10]={"On/Off GSM",\
                "Change baud rate UCxx(2,9600)",\
                "Change baud rate Arduino(3,9600)",
                "Enable/Disable Echo(4,1=on/0=off)",\
                "Configure URC = UART",\
                "Config Digital Audio NAU8814",\
                "Save Config",\
                "AT Command",\
                "Menu",\
              };
//String cmd_data;
void setup() 
{
  mySerial.begin(115200);
  Serial.begin(9600);
  print_menu();
}
void loop() 
{
 if (Serial.available())
  {
    //mySerial.write(Serial.read());
    String datain = Serial.readStringUntil('\n');
    int len_1 = datain.indexOf(F(","));
    int cmd_set = 255;
    if(len_1==-1)
    {
      cmd_set = datain.toInt();
      process_cmd(cmd_set,"");
    }
    else
    {
      cmd_set = datain.substring(0,len_1).toInt();
      String cmd_data = datain.substring(len_1+1);
      process_cmd(cmd_set,cmd_data);
  }
  }
  
  if (mySerial.available())
  {
    Serial.write(mySerial.read());
  }
  
}
void process_cmd(int x,String cmd_data_)
{
  switch(x)
  {
    case 1:
      Serial.println();
      Serial.print(">> ");
      Serial.println(CMD[x-1]);
      pinMode(START_PIN,OUTPUT);
      digitalWrite(START_PIN, HIGH);
      delay(1000);
      digitalWrite(START_PIN, LOW);
      delay(1000);
    break;
    case 2:
      Serial.println();
      Serial.print(">> ");
      Serial.print(CMD[x-1]);
      Serial.print("-->");
      Serial.println(cmd_data_);
      
      mySerial.println(F("AT+IFC=0,0"));
      delay(1000);
      mySerial.print(F("AT+IPR="));
      mySerial.println(cmd_data_);
      delay(1000);
      Serial.println("Change Baud rate Arduino");
      mySerial.begin(cmd_data_.toInt());
    break;
    case 3:
      Serial.println();
      Serial.print(">> ");
      Serial.print(CMD[x-1]);
      Serial.print("-->");
      Serial.println(cmd_data_);
      mySerial.begin(cmd_data_.toInt());
    break;
    case 4:
      Serial.println();
      Serial.print(">> ");
      Serial.print(CMD[x-1]);
      Serial.print("-->");
      Serial.println(cmd_data_);
      mySerial.print(F("ATE"));
      mySerial.println(cmd_data_);
    break;
    case 5:
      Serial.println();
      Serial.print(">> ");
      Serial.print(CMD[x-1]);
      Serial.print("-->");
      mySerial.println(F("AT+QURCCFG=\"urcport\",\"uart1\""));
     
    break;
    case 6:
      Serial.println();
      Serial.print(">> ");
      Serial.print(CMD[x-1]);
      Serial.print("-->");
      Serial.println(cmd_data_);
      mySerial.println(F("AT+QDAI=2"));
    break;
    case 7:
      Serial.println();
      Serial.print(">> ");
      Serial.print(CMD[x-1]);
      Serial.print("-->");
      Serial.println(cmd_data_);
      mySerial.println(F("AT&W"));
    break;
    case 8:
    Serial.println();
      Serial.print(">> ");
      Serial.print(CMD[x-1]);
      Serial.print("-->");
      Serial.println(cmd_data_);
      mySerial.println(cmd_data_);
    break;
    case 9:
    Serial.println();
      Serial.print(">> ");
      Serial.print(CMD[x-1]);
      Serial.print("-->");
      Serial.println(cmd_data_);
      print_menu();
    break;
  }
}
void print_menu()
{
  Serial.println();
  for(unsigned char i=0;i<MAX_CMD;i++)
  {
    Serial.print(i+1);
    Serial.print(") ");
    Serial.println(CMD[i]);
  }
}
