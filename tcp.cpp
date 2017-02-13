
#include "tcp.h"




TCP::TCP()
{	
}
bool TCP::Open(unsigned char contexid,unsigned char connectid,String service_type,String ip_url,String remote_port,String local_port,unsigned char access_mode)
{
	const long interval = 1000; 
	// AT+QIOPEN=1,0,"TCP","www.settakan.com",80,0,0
	bool ret = false;
	gsm.print("AT+QIOPEN=");
	gsm.print(String(contexid));
	gsm.print(",");
	gsm.print(String(connectid));
	gsm.print(",\"");
	gsm.print(String(service_type));
	gsm.print("\",\"");
	gsm.print(String(ip_url));
	gsm.print("\",");
	gsm.print(String(remote_port));
	gsm.print(",");
	gsm.print(local_port);
	gsm.print(",");
	gsm.print(String(access_mode));
	gsm.println("");
	
	unsigned long previousMillis = millis(); 
	while(1)
	{
		String req = gsm.readStringUntil('\n');
		gsm.debug(req);
		if(req.indexOf(F("OK"))!= -1)
			ret=true;
		if(req.indexOf(F("+QIOPEN:"))!= -1)
		{
			
			unsigned char index = req.indexOf(F(","));
			int ret_ = req.substring(index+1).toInt();
			int ret_2 = req.substring(index).toInt();
			//Serial.println(ret_);
			//Serial.println(ret_2);
			if((ret_==0)&&(ret_2==0))
			{
				
				ret=true;
				gsm.debug("Connect Server Success");
			}
			else
			{
				ret=false;
				gsm.debug("Connect Server Fail");
			}
			return(ret);
		}
		 unsigned long currentMillis = millis();
		if(currentMillis - previousMillis >= interval) 
		{
			gsm.print("AT+QIOPEN=");
			gsm.print(String(contexid));
			gsm.print(",");
			gsm.print(String(connectid));
			gsm.print(",\"");
			gsm.print(String(service_type));
			gsm.print("\",\"");
			gsm.print(String(ip_url));
			gsm.print("\",");
			gsm.print(String(remote_port));
			gsm.print(",");
			gsm.print(local_port);
			gsm.print(",");
			gsm.print(String(access_mode));
			gsm.println("");
			previousMillis = currentMillis;
		}			
	}
}
bool TCP::Open(String ip_url,String port)
{
	return(Open(1,0,"TCP",ip_url,port,"0",0));
}
bool TCP::StartSend(unsigned char contexid)
{
	const long interval = 3000; 
	gsm.print("AT+QISEND=");
	gsm.println(String(contexid));
	
	unsigned long previousMillis = millis(); 
	while(1)
	{
		if(gsm.available())
		{
			if(gsm.read()=='>')
			{
				gsm.debug("send raedy\r\n");
				return(true);
			}	
		}
		unsigned long currentMillis = millis();
		if(currentMillis - previousMillis >= interval) 
		{
			previousMillis = currentMillis;
			return(false);
		}			
		//String req = gsm.readStringUntil('>');
	}
}
bool TCP::StartSend(unsigned char contexid,int len)
{
	const long interval = 3000; 
	unsigned char flag_retry=0;
	gsm.print(F("AT+QISEND="));
	gsm.print(String(contexid));
	gsm.print(",");
	gsm.println(String(len));
	String buffer_="";
	unsigned long previousMillis = millis(); 
	unsigned long currentMillis;
	while(1)
	{
		if(gsm.available())
		{
			char c = gsm.read();
			//Serial.write(c);
			if(c =='>')
			{
				//gsm.debug("send ready");
				return(true);
			}	
			else
			{
				buffer_ += c;
				if(buffer_.indexOf(F("ERROR"))!=-1)
				{
					gsm.debug("Send Error\r\n");
					return(false);
				}
				if(buffer_.indexOf(F("NO CARRIER"))!=-1)
				{
					gsm.debug("NO CARRIER\r\n");
					return(false);
				}
				
				
			}
		/*	if(c =='E')
			{
				gsm.debug("send fail");
				return(false);
			}
*/			
		}
		currentMillis = millis();
		if(currentMillis - previousMillis >= interval) 
		{
			
			flag_retry++;
			gsm.println(F("AT"));
			gsm.wait_ok_ndb(1000);
			gsm.print(F("AT+QISEND="));
			gsm.print(String(contexid));
			gsm.print(",");
			gsm.println(String(len));
			
			previousMillis = currentMillis;
			if(flag_retry>=3)
			{
				gsm.debug("send error (timeout)\r\n");
				return(false);	
			}
			
		}			
		//String req = gsm.readStringUntil('>');
	}
}

bool TCP::StartSend()
{
	return(StartSend(0));
}

bool TCP::StopSend()
{
	
	gsm.write(0x1A);
	const long interval = 3000; 
	unsigned long previousMillis = millis(); 
	unsigned char cnt=0;
	while(!gsm.available())
	{
		unsigned long currentMillis = millis();
		if(currentMillis - previousMillis >= interval) 
		{
			gsm.write(0x1A);
			cnt++;
			if(cnt>3)
				return(false);
		}			
	}
	String req = gsm.readStringUntil('\n');
	if(req.indexOf(F("SEND OK"))!= -1)
	{
		return(true);	
	}
	else
	{
		return(false);
	}
}
bool TCP::WaitSendFinish()
{
	const long interval = 3000; 
	unsigned long previousMillis = millis(); 
	unsigned long currentMillis;
	unsigned char cnt=0;
	while(1)
	{
		currentMillis = millis();
		if(currentMillis - previousMillis >= interval) 
		{
			cnt++;
			if(cnt>3)
			{
				gsm.debug("Error unfinish");
				return(false);
			}
			previousMillis = currentMillis; 	
		}
		if(gsm.available())
		{
			String req = gsm.readStringUntil('\n');
			//gsm.debug(req);
			if(req.indexOf(F("SEND OK"))!= -1)
			{		
				return(true);	
			}
			if(req.indexOf(F("SEND FAIL"))!= -1)
			{		
				gsm.debug(req);
				return(false);	
			}
		}
			
	}
}
bool TCP::ReceiveAvailable()
{
	while(gsm.available())
	{
		String req = gsm.readStringUntil('\n');
		if(req.indexOf(F("+QIURC: \"recv\""))!= -1)
		{
			unsigned char index = req.indexOf(F(","));
			ReceiveConnectID = req.substring(index+1).toInt();	
			return(true);
		}
		else
		{
			return(false);
		}
	}
}
int TCP::ReadBuffer()
{
	return(ReadBuffer(1500));
}
int TCP::ReadBuffer(unsigned char contexid , int max_len)
{
	const long interval = 3000; 
	unsigned long previousMillis = millis(); 
	unsigned long currentMillis ; 
	unsigned char flag=0;  
		
	gsm.print(F("AT+QIRD="));
	gsm.print(String(contexid));
	gsm.print(",");
	gsm.println(String(max_len));
	while(1)
	{
		currentMillis = millis();
		if(gsm.available())
		{
			String req = gsm.readStringUntil('\n');
			if(req.indexOf(F("+QIRD:"))!= -1)
			{
				int index = req.indexOf(F(" "));
				//Serial.println(req);
				return(req.substring(index+1).toInt());	
			}
		}
		if(currentMillis - previousMillis >= interval)
		{
			if(flag++==3)
				return(0);
			//Serial.println("timeout");
			gsm.print(F("AT+QIRD="));
			gsm.print(String(contexid));
			gsm.print(",");
			gsm.println(String(max_len));
		}
	}
}
int TCP::ReadBuffer(int max_len)
{
	return(ReadBuffer(0,max_len));
}
bool TCP::CheckConnection(unsigned char query_type , unsigned char contexid )
{
	gsm.print("AT+QISTATE=");
	gsm.print(String(query_type));
	gsm.print(",");
	gsm.println(String(contexid));
	
	//gsm.println("AT+QISTATE=1,0");
	unsigned char flag=0;
	while(1)
	{
		if(gsm.available())
		{
			String req = gsm.readStringUntil('\n');
			gsm.debug(req);
			if(req.indexOf(F("+QISTATE:"))!= -1)
			{
				flag+=5;
			}
			if(req.indexOf(F("OK"))!= -1)
			{
				flag+=2;
			}
			if(flag==7)
					return(true);
			else if(flag>3) 
				return(false);
		}
	}
}

bool TCP::CheckConnection()
{
	return(CheckConnection(1,0));
}
bool TCP::Close(unsigned char contexid)
{
	gsm.print("AT+QICLOSE=");
	gsm.println(String(contexid));
	//gsm.println("AT+QICLOSE=0");
	//while(!gsm.available()){}
	while(gsm.available())
	{
			String req = gsm.readStringUntil('\n');
			//Serial.println(req);
			if(req.indexOf(F("OK"))!= -1)
			{
				return(true);
			}
			else
			{
				return(false);
			}
	}
	
}
bool TCP::Close()
{
	return(Close(0));
}
void TCP::Ping(unsigned char contextid,String ip_url)
{
	//AT+QPING=1,“www.baidu.com”
	gsm.print("AT+QPING=");
	gsm.print(String(contextid));
	gsm.print(",\"");
	gsm.print(ip_url);
	gsm.println("\"");
	
}
String TCP::NTP(unsigned char contexid,String ip_url,String port)
{
	gsm.print("AT+QNTP=");
	gsm.print(String(contexid));
	gsm.print(",\"");
	gsm.print(ip_url);
	gsm.print("\",");
	gsm.println(port);
	const long interval = 3000; 
	unsigned long previousMillis = millis(); 
	while(1)
	{
		if(gsm.available())
		{
			String req = gsm.readStringUntil('\n');
			if(req.indexOf(F("+QNTP:"))!= -1)
			{
				return(req);
			}
			
		}
		unsigned long currentMillis = millis();
		if(currentMillis - previousMillis >= interval) 
		{
			return("");
		}			
	}
}
//AT+QNTP=1,"time.navy.mi.th",123
//AT+QNTP=1,"203.185.67.115",123
//AT+QNTP=1,"158.108.212.149",123
//AT+QNTP=1,"203.185.69.60",123
//AT+QNTP=1,"202.28.18.72",123

//AT+QICLOSE=0    
//AT+CCLK?   


void TCP::write(char data)
{
	gsm.write(data);
}
void TCP::print(int data)
{
	gsm.print(String(data));
}
void TCP::println(int data)
{
	gsm.println(String(data));
}
void TCP::print(String data)
{
	gsm.print(data);
}
void TCP::println(String data)
{
	gsm.println(data);
}
