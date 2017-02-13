#include "File.h"

void func_null(String data){}
void func_null(char data){}

UC_FILE::UC_FILE()
{
		
}
bool available_=true;

void UC_FILE :: begin()
{
	ListOutput = func_null;
	DataOutput = func_null;
}
long UC_FILE :: GetSpace(String pattern)
{
	return(space(pattern ,0));
}
long UC_FILE :: GetFreeSpace(String pattern)
{
	return(space(pattern ,1));
} 
long UC_FILE:: space(String pattern , unsigned char mode)//Mode 0 = All Space,1 = Free Space
{
	char arr[20];
	gsm.print(F("AT+QFLDS="));
	gsm.println("\""+pattern+"\"");
	while(!gsm.available())
	{}
	gsm.start_time_out();
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	   
	   if(req.indexOf(F("+QFLDS:")) != -1)
		{
			char index1,index2;
			if(mode)//FreeSpace
			{
				index1 = req.indexOf(F(" "));
				index2 = req.indexOf(F(","));
				String str_l =req.substring(index1+1,index2);
				str_l.toCharArray(arr,sizeof(arr));
				
			}
			else//Space
			{
				char index1 = req.indexOf(F(","));
				String str_l =req.substring(index1+1);
				str_l.toCharArray(arr,sizeof(arr));
			}			
			gsm.wait_ok(1000);
			return(atol(arr));
		}
		if(gsm.time_out(20000))
		{
			return(-1);
		}
	}
	return(-1);
}
void UC_FILE:: List(String pattern)
{
	//AT+QFLST="*"
	//AT+QFLST="RAM:*"
	if(pattern == UFS)
	gsm.println(F("AT+QFLST=\"*\""));
	if(pattern == RAM)
	gsm.println(F("AT+QFLST=\"RAM:*\""));	
	while(!gsm.available())
	{}
	available_=true;
	
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	   
	    if(req.indexOf(F("OK")) != -1)
		{
				return;
		}
		if(req.indexOf(F("ERROR")) != -1)
		{
			return;
		}
		(*ListOutput)(req);
	}
	
	
}
long UC_FILE:: List(String pattern,String fn)
{
	char arr[20];
	if(pattern == UFS)
	{
		gsm.print(F("AT+QFLST=\""));
		gsm.print(fn);	   
		gsm.println(F("\""));	
	}
	if(pattern == RAM)
	{
		//AT+QFLST="RAM:1"
		gsm.print(F("AT+QFLST=\"RAM:"));
		gsm.print(fn);
		gsm.println("\"");
	}
	//+QFLST: "RAM:1",0
	while(!gsm.available())
	{}
	gsm.start_time_out();
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	   
	    if(req.indexOf(F("+QFLST:")) != -1)
		{
				char index1 = req.indexOf(F(","));
				String str_l =req.substring(index1+1);
				str_l.toCharArray(arr,sizeof(arr));
				gsm.wait_ok(1000);
				return(atol(arr));
		}
		if(req.indexOf(F("ERROR")) != -1)
		{
			return(-1);
		}		
	}
	return(-1);

}
bool UC_FILE:: available()
{
	return(available_);
}
String UC_FILE:: ReadLine()
{
	String req = gsm.readStringUntil('\n');
	if(req.indexOf(F("OK")) != -1)
	{
		available_=false;
	}
	(*ListOutput)(req);
	return(req);	
}
bool UC_FILE:: Delete(String pattern,String fn)
{
	gsm.print(F("AT+QFDEL=\""));
	if(pattern == UFS)
	{
		gsm.print(fn);
	}
	
	if(pattern == RAM)
	{
		gsm.print(F("RAM:"));
		gsm.print(fn);
	}
	gsm.println(F("\""));
	gsm.wait_ok(5000);
}
int UC_FILE::Open(String pattern,String fn) 
{
	gsm.print(F("AT+QFOPEN=\""));
	if(pattern == UFS)
	{
		gsm.print(fn);
	}
	if(pattern == RAM)
	{
		gsm.print(F("RAM:"));
		gsm.print(fn);
	}
	gsm.println(F("\","));
	while(!gsm.available())
	{}
	gsm.start_time_out();
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	   
	    if(req.indexOf(F("+QFOPEN:")) != -1)
		{
			char index1 = req.indexOf(F(" "));
			char index2 = req.indexOf(F("\r\n"));
			String str =req.substring(index1+1,index2);
			//Serial.print("OK xxx");
			//Serial.print(str);
			gsm.wait_ok(1000);
			return(str.toInt());
		}
		if(req.indexOf(F("ERROR")) != -1)
		{
			Serial.print("Error");
			return(-1);
		}
		
	}

}

bool UC_FILE:: Close(int handle)
{
	gsm.print(F("AT+QFCLOSE="));
	gsm.print(handle,DEC);
	gsm.println("");
	gsm.wait_ok(5000);
}

bool UC_FILE:: Close_(int handle)
{
	gsm.print(F("AT+QFCLOSE="));
	gsm.print(handle,DEC);
	gsm.println("");
	gsm.wait_ok_ndb(5000);
}

bool UC_FILE:: BeginWrite(int handle,int size)
{
	//AT+QFWRITE=0,10
	gsm.print(F("AT+QFWRITE="));
	gsm.print(handle,DEC);
	gsm.print(F(","));
	gsm.print(size,DEC);
	gsm.println("");
	
	while(!gsm.available())
	{}
	gsm.start_time_out();
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	   
	    if(req.indexOf(F("CONNECT")) != -1)
		{
			return(true);
		}
		if(req.indexOf(F("ERROR")) != -1)
		{
			return(false);
		}		
	}

	//return(gsm.wait_ok(5000));
}
void UC_FILE:: Write(char data)
{
	gsm.write(data);
}
void UC_FILE:: Print(String data)
{
	gsm.print(data);
}
void UC_FILE:: Println(String data)
{
	gsm.println(data);
}
bool UC_FILE::WaitFinish()
{
	return(gsm.wait_ok(10000));
}
bool UC_FILE::SeekAtStart(int handle)
{
	return(Seek(handle,0));
}
bool UC_FILE::Seek(int handle,long start_at)
{
	//AT+QFSEEK=0,0,0
	gsm.print(F("AT+QFSEEK="));
	gsm.print(handle,DEC);
	gsm.print(F(","));
	gsm.print(start_at,DEC);
	gsm.print(F(","));
	gsm.println(F("0"));	
	return(gsm.wait_ok(1000));
}
int UC_FILE::Read(int handle,int buf_size,char *buf)
{
	//AT+QFREAD=0,10
	int size;
	gsm.print(F("AT+QFREAD="));
	gsm.print(handle,DEC);
	gsm.print(F(","));
	gsm.print(buf_size,DEC);
	gsm.println("");
	while(!gsm.available())
	{}
	unsigned char flag=1;
	while(flag)
	{
		String req = gsm.readStringUntil('\n');	
	   
	    if(req.indexOf(F("CONNECT")) != -1)
		{
			char index1 = req.indexOf(F(" "));
			char index2 = req.indexOf(F("\r\n"));
			String str =req.substring(index1+1,index2);
			//Serial.println("size = "+str);
			size=str.toInt();
			
			flag=0;
		}
		if(req.indexOf(F("ERROR")) != -1)
			return(-1);
	}
	flag = 1;
	int cnt=0;
	while(flag)
	{
		if(gsm.available())
		{
			char c = gsm.read();
			buf[cnt] = c;
			//Serial.write(c);
			cnt++;
			if(cnt>=size)
			{
				flag=0;
			}
		}
	}
	gsm.wait_ok_ndb(1000);
	return(size);	
}

void UC_FILE::ReadFile(String pattern,String file_name)
{
	int handle = Open(pattern,file_name);
	if(handle!=-1)
	{
		SeekAtStart(handle);
		int buf_size=100;
		char buf[buf_size];
		int size_ = Read(handle,buf_size,buf);
		while(size_!=-1)
		{
			for(int i=0;i<size_;i++)
			{
				//Serial.write(buf[i]);
				(*DataOutput)(buf[i]);
			}
			size_ = Read(handle,buf_size,buf);
		}
   }
   Close_(handle);
}





























