#include "TEE_UC20.h"
#include "SoftwareSerial.h"
#include <AltSoftSerial.h>
#include "File.h"
UC_FILE file;

//SoftwareSerial mySerial(8, 9); // RX, TX
AltSoftSerial mySerial;
String File_name = "test123.txt";

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
  delay(3000);
  Serial.println("Start");
  gsm.Event_debug = debug;
  Serial.println(F("UC20"));
  Serial.println(F("Wait To Ready ...."));
  gsm.PowerOn(); 
  while(gsm.WaitReady()){}
  Serial.println(F("Ready"));
  
  file.begin();
  get_space(UFS);
  file.Delete(UFS,File_name);
  //file.Delete(UFS,"*"); //Delete All File
  Serial.println(F("Write Data..."));
  write_file(File_name,F("Hello\r\nTest Write File To UFS"));
  Serial.println(F("Read Data..."));
  read_file(UFS,File_name);
  Serial.println();
  Serial.println("List File...");
  list_file();
  Serial.println();
}

void list_file()
{
  file.ListOutput = debug;
  file.List(UFS);
  Serial.println();
}
void get_space(String pattern)
{
  long space_size = file.GetSpace(pattern);
  Serial.print("Get All Space = ");
  Serial.println(space_size);
  Serial.print("Get Free Space = ");
  space_size = file.GetFreeSpace(pattern);
  Serial.println(space_size);
}

void read_file(String pattern,String file_name)
{
  file.DataOutput =  data_out;
  file.ReadFile(pattern,file_name);
}


void write_file(String file_name,String data)
{
  int handle = file.Open(UFS,file_name);
  if(handle!=-1)
  {
    if(file.BeginWrite(handle,data.length()))
    {
      file.Print(data);
      file.WaitFinish();
    }
  }
  file.Close(handle);
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
