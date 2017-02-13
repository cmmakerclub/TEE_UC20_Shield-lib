#include "gnss.h"


GNSS::GNSS(){}

bool GNSS:: Start()
{
	gsm.println(F("AT+QGPS=1"));
	return(gsm.wait_ok(3000));
}

bool GNSS:: EnableNMEA()
{
	gsm.println(F("AT+QGPSCFG=\"nmeasrc\",1"));
	return(gsm.wait_ok(3000));
}
bool GNSS:: DisableNMEA()
{
	gsm.println(F("AT+QGPSCFG=\"nmeasrc\",0"));
	return(gsm.wait_ok(3000));
}

bool GNSS:: Stop()
{
	gsm.println(F("AT+QGPSEND"));
	return(gsm.wait_ok(3000));
}
String GNSS:: GetNMEA(String nmea)
{
	//AT+QGPSGNMEA=“GGA” 
	String data= "Please Wait...";
	gsm.print(F("AT+QGPSGNMEA=\""));
	gsm.print(nmea);
	gsm.println(F("\""));
	while(!gsm.available())
	{}
	
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	    //gsm.debug(req);
		if(req.indexOf(F("+QGPSGNMEA:")) != -1)
		{
			//return(req);
			char index = req.indexOf(F("$"));
			data = req.substring(index);
		}
		if(req.indexOf(F("OK")) != -1)
		{
			return(data);
		}
		
	}			
	
}
String GNSS:: GetPosition()
{
	gsm.println(F("AT+QGPSLOC?"));
	//+CME ERROR: 516  no fix
	unsigned long previousMillis_timeout = millis();
	unsigned long currentMillis;
	while(!gsm.available())
	{
		currentMillis = millis();
		if(currentMillis - previousMillis_timeout >= 3000) 
		{
			return(F("Wait.."));
		}
	}
//gsm.debug("pass");
	previousMillis_timeout = millis();
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	  if(req.indexOf(F("+QGPSLOC:")) != -1)
		{
			return(req);
		}
		if(req.indexOf(F("+CME ERROR:")) != -1)
		{
			if(req.indexOf(F("516")) != -1)
			{
					return(F("Not fixed now.\r\n"));
			}
			
		}	
		currentMillis = millis();
		if(currentMillis - previousMillis_timeout >= 3000) 
		{
			return(F("Time Out"));
		}
	}			
}