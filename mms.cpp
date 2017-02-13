#include "mms.h"

MMS::MMS(){}

bool MMS:: SetMMSC(String data)
{
	//AT+QMMSCFG="mmsc","http://mms.trueh.com:8002" 
	gsm.print(F("AT+QMMSCFG=\"mmsc\",\""));
	gsm.print(data);
	gsm.println("\"");
	return(gsm.wait_ok(3000));
}
bool MMS:: SetProxy(String ip,String port)
{
	//AT+QMMSCFG="proxy","10.4.7.39",8080
	gsm.print(F("AT+QMMSCFG=\"proxy\",\""));
	gsm.print(ip);
	gsm.print("\",");
	gsm.println(port);
	if(!gsm.wait_ok(3000))
		return(false);
	gsm.println(F("AT+QMMSCFG=\"sendparam\",6,2,0,0,0,4")); 
	return(gsm.wait_ok(3000));
}
bool MMS:: Title(String title)
{
	//AT+QMMSCFG="character","ASCII"
	gsm.println(F("AT+QMMSCFG=\"character\",\"ASCII\""));
	if(!gsm.wait_ok(3000))
		return(false);
	//AT+QMMSEDIT=4,1,"test uc15 mms" 
	gsm.print(F("AT+QMMSEDIT=4,1,\""));
	gsm.print(title);
	gsm.println("\"");
	return(gsm.wait_ok(3000));
}
bool MMS:: SendTo(String receive)
{
	//AT+QMMSEDIT=1,1,"08xxxxxxxx" 
	gsm.print(F("AT+QMMSEDIT=1,1,\""));
	gsm.print(receive);
	gsm.println(F("\""));
	return(gsm.wait_ok(3000));
}
bool MMS:: AddFile(String pattern,String Filename)
{
	if(pattern == UFS)
	gsm.print(F("AT+QMMSEDIT=5,1,\""));
	if(pattern == RAM)
	gsm.print(F("AT+QMMSEDIT=5,1,\"RAM:"));	
	gsm.print(Filename);
	gsm.println(F("\""));
	return(gsm.wait_ok(3000));
}
String MMS::Send()
{
	gsm.println(F("AT+QMMSEND=500"));
	//+QMMSEND: 0,200 
	while(!gsm.available())
	{}
	gsm.start_time_out();
	String req;
	while(1)
	{
		req = gsm.readStringUntil('\n');	
	    if(req.indexOf(F("+QMMSEND:")) != -1)
		{
			return(req);
		}
		if(req.indexOf(F("ERROR")) != -1)
		{
			return(req);
		}		
	}
	return(req);
}
bool MMS::Clear()
{
	gsm.println(F("AT+QMMSEDIT=0"));
	return(gsm.wait_ok(3000));
}
String MMS::ListMMSFile()
{
	String data;
	gsm.println(F("AT+QMMSEDIT=5"));
	while(!gsm.available())
	{}
	gsm.start_time_out();
	String req;
	while(1)
	{
		req = gsm.readStringUntil('\n');	
	    data += req+"\r\n";
		if(req.indexOf(F("OK")) != -1)
		{
			return(data);
		}		
	}
	return(data);
	 
}





























