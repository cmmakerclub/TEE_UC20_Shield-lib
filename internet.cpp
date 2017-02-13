#include "internet.h"



INTERNET:: INTERNET(){}

bool INTERNET:: Configure(String apn,String user,String password)
{
	String 	str = "AT+QICSGP=1,1,";
			str += "\""+apn+"\",";
			str	+= "\""+user+"\",";
			str	+= "\""+password+"\",";
			str	+= "1";
	gsm.println(str);
	return(gsm.wait_ok(10000));
}
bool INTERNET:: Connect()
{
	gsm.println(F("AT+QIACT=1"));
	return(gsm.wait_ok(10000));
}
bool INTERNET:: DisConnect()
{
	gsm.println(F("AT+QIDEACT=1"));
	return(gsm.wait_ok(10000));
}
String INTERNET:: GetIP()
{
	gsm.println(F("AT+QIACT?"));
	while(!gsm.available())
	{}
	gsm.start_time_out();
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	   
	   if(req.indexOf(F("+QIACT:")) != -1)
		{
			char index1 = req.indexOf(F("\""));
			char index2 = req.indexOf(F("\""),index1+1);
			gsm.wait_ok(1000);
			return(req.substring(index1+1,index2));
		}
		if(gsm.time_out(20000))
		{
			return(F("GetIP Timeout"));
		}
	}
	return(F(""));
}







