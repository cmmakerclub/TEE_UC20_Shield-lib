#include "TEE_UC20.h"
#include "SoftwareSerial.h"
#include <AltSoftSerial.h>
#include "internet.h"
#include "File.h"
#include "mms.h"
#include <SPI.h>
#include <SD.h>

#define SD_CS 5
#define PIC_FILE "pic.jpg"
#define TEXT_FILE "text.txt"
#define RECEIVE_NUMBER "0909877492"

INTERNET net;
UC_FILE file;
MMS mms;
File myFile;

//SIM TRUE  MMS
#define APN   "hmms"
#define USER  "true"
#define PASS  "true"
#define MMSC  "http://mms.trueh.com:8002"
#define PROXY "10.4.7.39"
#define PORT  "8080"

//SoftwareSerial mySerial(8, 9); // RX, TX
AltSoftSerial mySerial;

void debug(String data)
{
  Serial.println(data);
}
void setup() 
{
  Serial.begin(9600);
  file.begin();
  gsm.begin(&mySerial,9600);
  delay(3000);
  Serial.println(F("Start"));
  gsm.Event_debug = debug;
  Serial.println(F("UC20"));
  Serial.println(F("Wait To Ready ...."));
  gsm.PowerOn(); 
  while(gsm.WaitReady()){}
  
  Serial.println(F("Ready"));
  if (!SD.begin(SD_CS)) 
  {
    Serial.println(F("initialization failed!"));
    return;
  }
  Serial.println(F("initialization done."));
  
  Serial.println(F("Delete All File in UFS"));
  file.Delete(UFS,"*"); 
 
  Serial.println(F("Copy File From SD Card to UFS"));
  copy_file_from_SD_to_Flash(PIC_FILE);
 
  Serial.println(F("Create Text File"));
  write_file(TEXT_FILE,F("Hello Test MMS form UC20"));
 
  Serial.println(F("List File In UFS"));
  file.ListOutput=debug;
  file.List(UFS);
 
  Serial.println(F("Disconnect Internet"));
  net.DisConnect();
 ////////////////////////////////Config SMS Server///////////////////////////
  Serial.println(F("Config MMS"));
  net.Configure(APN,USER,PASS);
  
  Serial.println(F("Connect SMS Server"));
  net.Connect();
  Serial.println(F("Get My IP"));
  Serial.println(net.GetIP());
  Serial.println(F("Set MMSC"));
  mms.SetMMSC(MMSC);
  Serial.println(F("Set Proxy"));
  mms.SetProxy(PROXY,PORT);
  Serial.println(F("Set Receive Phonenumber"));
  mms.SendTo(RECEIVE_NUMBER);
  
  Serial.println(F("Set Title MMS"));
  mms.Title(F("Test MMS From UC20"));
  Serial.println(F("Add MMS File"));
  mms.AddFile(UFS,PIC_FILE);
  mms.AddFile(UFS,TEXT_FILE);
  Serial.println(F("List MMS File"));
  Serial.println(mms.ListMMSFile());
  Serial.println(F("Send MMS"));
  Serial.println( mms.Send());
  Serial.println(F("Disconnect"));
  net.DisConnect();
  Serial.println(F("Clear MMS Profile"));
  mms.Clear();
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
void read_file(String file_name)
{
  int handle = file.Open(UFS,file_name);
  if(handle!=-1)
  {
    file.SeekAtStart(handle);
    int buf_size=100;
    char buf[buf_size];
    Serial.println();
    int size_ = file.Read(handle,buf_size,buf);
    while(size_!=-1)
    {
      for(int i=0;i<size_;i++)
      {
        Serial.write(buf[i]);
      }
      size_ = file.Read(handle,buf_size,buf);
    }
   }
   file.Close(handle);
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
void copy_file_from_SD_to_Flash(String filename)
{
  char fn[10] ;
  filename.toCharArray(fn,filename.length()+1);
  Serial.println(fn);
  myFile = SD.open(fn);
  Serial.print("\r\n File Size =");
  Serial.println(myFile.size());
  int handle;
  long copy_size=0;
  if (myFile) 
  {
     handle = file.Open(UFS,filename);
     if(handle!=-1)
     {
         file.BeginWrite(handle,myFile.size());
     }
      while (myFile.available()) 
     {
        char c = myFile.read();
        Serial.println(copy_size);
        copy_size++;
        file.Write(c);
     }
     file.WaitFinish();
  }
  file.Close(handle);
  myFile.close();
  Serial.println("\r\nEnd Copy");
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
