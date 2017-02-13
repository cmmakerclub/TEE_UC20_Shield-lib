#include "call.h"
#include "TEE_UC20.h"

CALL:: CALL(){}

unsigned char CALL:: Call(String call_number)  /* return 	0 =	Timeout
										return  1 = OK
										return  2 = NO CARRIER
										return  3 = BUSY
									*/
{
	gsm.print(F("ATD"));
	gsm.print(call_number);
	gsm.println(F(";"));
	while(!gsm.available())
	{}
	gsm.start_time_out();
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	    if(req.indexOf(F("OK")) != -1)
		{
			gsm.debug(F("OK"));
			return(1);
		}
			
		if(req.indexOf(F("NO CARRIER")) != -1)
		{
			gsm.debug(F("NO CARRIER"));
			return(2);
		}
		if(req.indexOf(F("BUSY")) != -1)
		{
			gsm.debug(F("BUSY"));
			return(2);
		}
			
		if(gsm.time_out(10000))
		{
			gsm.debug(F("Error"));
			return(0);
		}
	}
	return(0);
}

bool CALL:: Answer()
{
	gsm.println(F("ATA"));
	return(gsm.wait_ok(3000));
}
bool CALL:: DisconnectExisting()
{
	gsm.println(F("ATH"));
	return(gsm.wait_ok(3000));
}
bool CALL:: HangUp()
{
	gsm.println(F("AT+CHUP"));
	return(gsm.wait_ok(3000));
}
String CALL:: CurrentCallsMe()
{
	gsm.println(F("AT+CLCC"));
	while(!gsm.available())
	{}
	gsm.start_time_out();
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	   
	   if(req.indexOf(F("+CLCC")) != -1)
		{
			char index1 = req.indexOf(F("\""));
			char index2 = req.indexOf(F("\""),index1+1);
			return(req.substring(index1+1,index2));
			//Serial.println("req");
			return(req);
		}
		if(gsm.time_out(20000))
		{
			return(F("CurrentCallsMe Timeout"));
		}
	}
	return(F(" "));	
} 
bool CALL::WaitRing()
{
	while(gsm.available())
	{
		String req = gsm.readStringUntil('\n');	
	   
	   if(req.indexOf(F("RING")) != -1)
		{
			return(true);
		}	
	}
	return(false);
}










