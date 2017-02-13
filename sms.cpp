#include "sms.h"



SMS::SMS(){}


void SMS::DefaultSetting()
{
	gsm.println(F("AT+CMGF=1"));
	gsm.debug(F("AT+CMGF=1"));
	gsm.wait_ok(3000);
	gsm.println(F("AT+CSMP=17,167,0,8"));
	gsm.debug(F("AT+CSMP=17,167,0,8"));
	gsm.wait_ok(3000);
	gsm.println(F("AT+CSCS=\"UCS2\""));
	gsm.debug(F("AT+CSCS=\"UCS2\""));
	gsm.wait_ok(3000);
}
void SMS::Start(String rx_number)
{
	gsm.print(F("AT+CMGS=\""));
	unsigned char i=0;
	while(rx_number[i])
	{
		gsm.print(F("00"));
		gsm.print(rx_number[i],HEX);
		i++;
	}
	gsm.println(F("\""));
	while(!gsm.available())
	{}
	String req = gsm.readStringUntil('>');
	
}
void SMS::Send(String data)
{
	unsigned char i=0;
	String buf="";
	while(data[i])
	{
		unsigned char c = data[i]&0xFF;
		if(c==0xE0)
		{
			gsm.print(F("0E"));
			//gsm.debug("0E");
			i++;
			c = data[i];
			if(c == 0xB8)
			{
				i++;
				c = data[i]-0x80;
				if(c <= 0x0F)
				{
					gsm.print(F("0"));
					//gsm.debug(F("0"));					
				}
				buf = String(c,HEX);
				buf.toUpperCase();
				gsm.print(buf);
				//gsm.debug(buf);
			}
			else
			{
				i++;
				c = data[i]-0x40;
				if(c <= 0x0F)
				{
					gsm.print(F("0"));
					//gsm.debug(F("0"));
				}
				buf = String(c,HEX);
				buf.toUpperCase();
				gsm.print(buf);
				//gsm.debug(buf);
			}			
		}
		else
		{

			gsm.print(F("00"));
			if(c == 0x0A)
			{
			 gsm.print("0A");
			// gsm.debug("0A");
			}
			else if(c == 0x0D)
			{
			 gsm.print("0D");
			// gsm.debug("0D");
			}
			else
			{
				buf = String(c,HEX);
				buf.toUpperCase();
				gsm.print(buf);
				//gsm.debug(buf);
			}
				
		}
		i++;
	}
	
}
void SMS::Sendln(String data)
{
	Send(data+"\r\n");
}
void SMS::Stop()
{
	gsm.write(0x1A);
	if(gsm.wait_ok(10000))
	{
		gsm.debug("Send OK");
	}
}
unsigned char SMS:: IndexNewSMS()
{
	return(gsm.index_new_SMS);
}

String SMS:: ReadSMS(int index)
{
	gsm.println(F("AT+CMGF=1"));
	gsm.wait_ok(10000);
	gsm.print(F("AT+CMGR="));
	gsm.print(index,DEC);
	gsm.println("");
    String data;
	while(1)
	{
		if(gsm.available())
		{
			String req = gsm.readStringUntil('\n');
			if(req.indexOf(F("+CMGR"))!= -1)
			{
				SMSInfo = req;	
			}
			else if(req.indexOf(F("OK"))!= -1)
			{
				break;
			}
			else
			{
				if((req!="\r")&&(req!="\n"))
					data+=req;
			}
		}
	}
	return(data);
}
bool SMS:: DeleteSMS(int index)
{
	gsm.print(F("AT+CMGD="));
	gsm.print(index,DEC);
	gsm.println("");
	return(gsm.wait_ok(10000));
	
}

String SMS::ConvertStrUnicodeToTIS620(String data)
{
  int i=0;
  char c;
  String output="";
  unsigned char flag_th=0;
  while(data[i])
  {
	if((data[i]=='0')&&(data[i+1]=='E'))
	{
		flag_th=1;
	}
	else
	{
	    output+=data[i];
		output+=data[i+1];
	}
	i+=2;  
	if(i>data.length())
    break;
  }
  if(!flag_th)
	  return(output);
  else
	  output="";
  
  i=0;
  while(data[i])
  {
    if((data[i]=='0')&&(data[i+1]=='0'))
    {
      c  = (data[i+2]-0x30)<<4;
	  if((data[i+3])>='A')
		  c |= (data[i+3]-0x30)-0x07;
	  else
      c |= (data[i+3]-0x30);
	  output+=c;
    }
    if((data[i]=='0')&&(data[i+1]=='E'))
    {
      c  = (data[i+2]-0x30)<<4;
      c += 0xA0;
	  if((data[i+3])>='A')
	  {
		  c |= (data[i+3]-0x30)-0x07;
	  }
	  else
	  c |= (data[i+3]-0x30);  
	  output+=c;
	      }
    i+=4;
    if(i>data.length())
    break;
  }
  return(output);
}

String SMS:: ConvertStrUnicodeToUTF8(String data)
{
	int i=0,j=0;
	String dat = ConvertStrUnicodeToTIS620(data);
	//Serial.println(dat);
	char out[dat.length()*3];
	while(dat[i])
	{
		unsigned char c = dat[i];
		//Serial.print(c,HEX);
		if(c >= 0xA0)
		{
			out[j] = 0xE0;
			j++;
			if(c >=0xE0)
			{
				out[j] = 0xB9;
				j++;
				out[j] = c-0x60;
				j++;
			}
			else	
			{
				out[j] = 0xB8;
				j++;
				out[j] = c-0x20;
				j++;
			}
		}
		else
		{
			out[j] = c;
			j++;
		}
		
		i++;
	}
	out[j]=0;
//for(int x=0;x<j;x++)
//		Serial.print(out[x],HEX);
//Serial.println();
	return(String(out));	
}




















