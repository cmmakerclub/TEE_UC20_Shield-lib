#include "TEE_UC20.h"

int START_PIN = 4;
UC20 gsm;
unsigned long previousMillis_timeout = 0;
void event_null(String data){}
UC20::UC20()
{
	Event_debug =  event_null;
}
void UC20:: begin(SoftwareSerial *serial,long baud)
{
	serial->begin(baud);
	_Serial = serial;
}

void UC20:: begin(HardwareSerial *serial,long baud)
{
	serial->begin(baud);
	_Serial = serial;
}
#if ATLSOFTSERIAL
void UC20:: begin(AltSoftSerial *serial,long baud)
{
	serial->begin(baud);
	_Serial = serial;
}
#endif
void UC20:: debug (String data)
{
	(*Event_debug)(data);
}

void UC20::SetPowerKeyPin(int pin)
{
	START_PIN = pin;
}
bool UC20::PowerOn()
{

	String req="";
	//_Serial->println(F("AT"));

	pinMode(START_PIN, OUTPUT);

	digitalWrite(START_PIN, HIGH);
	delay(1000);
	digitalWrite(START_PIN, LOW);
	delay(1000);





	/*
	while(!_Serial->available())
	{
		delay(1);
	}
	start_time_out();
	*/

	unsigned long pv_out = millis();

	while(1)
	{
		if(_Serial->available())
		{
			req = _Serial->readStringUntil('\n');
			// debug(req);
			if(req.indexOf(F("RDY")) != -1)
			{
				start_time_out();
				debug(F("Power ON"));
				return(true);
			}
			if(req.indexOf(F("POWERED DOWN")) != -1)
			{
				//start_time_out();
				debug(F("Power OFF"));
				pinMode(START_PIN, OUTPUT);
				digitalWrite(START_PIN, HIGH);
				delay(1000);
				digitalWrite(START_PIN, LOW);
				delay(1000);
			}

		}
		unsigned long current_out = millis();
			//debug("x");
			if(current_out - pv_out >= (10000))
		{
				digitalWrite(START_PIN, HIGH);
				delay(1000);
				digitalWrite(START_PIN, LOW);
				delay(1000);
				debug(F("Power Retry"));
				pv_out = current_out;
		}
	}
}
bool UC20::PowerOff()
{
	pinMode(START_PIN, OUTPUT);
	digitalWrite(START_PIN, HIGH);
    delay(1000);
	digitalWrite(START_PIN, LOW);
	delay(1000);
	while(!_Serial->available())
	{
		delay(1);
	}
	while(1)
	{
		String req = _Serial->readStringUntil('\n');
		//debug(req);
	    if(req.indexOf(F("RDY")) != -1)
		{
			debug(F("Power ON"));
			pinMode(START_PIN, OUTPUT);
			digitalWrite(START_PIN, HIGH);
			delay(1000);
			digitalWrite(START_PIN, LOW);
			delay(1000);
		}
		if(req.indexOf(F("POWERED DOWN")) != -1)
		{
			debug(F("Power OFF"));
			return(true);
		}
	}

}
bool UC20::WaitReady()
{
	start_time_out();
	while(_Serial->available())
	{
		String req = _Serial->readStringUntil('\n');
		debug(req);
	    if(req.indexOf(F("PB DONE")) != -1)
		{
			debug(F("\r\nUC20 Ready..."));
			debug(F("\r\nClose Echo"));
			_Serial->println(F("ATE0"));
			wait_ok(1000);

			return(false);
		}
		else if(req.indexOf(F("POWERED DOWN")) != -1)
		{
			digitalWrite(START_PIN, HIGH);
			delay(1000);
			digitalWrite(START_PIN, LOW);
			delay(1000);
			PowerOn();
		}
		else
		{
			return(true);
		}
		if(time_out(30000))
		{
			debug(F("\r\nNo response--> Please check Hardware"));
		}
	}
	return(true);
}
String UC20::GetOperator()
{
	_Serial->println(F("AT+COPS?"));
	while(!_Serial->available())
	{}
	start_time_out();
	while(1)
	{
		String req = _Serial->readStringUntil('\n');
		if(req.indexOf(F("+COPS")) != -1)
		{
			//+COPS: 0,0,"TRUE-H",2
			/*char comma1 = req.indexOf(F(","));
			char comma2 = req.indexOf(F(","),comma1+1);
			char comma3 = req.indexOf(F(","),comma2+1);
			String  operate_name = req.substring(comma2+2,comma3-1);
			String  acc_tech = req.substring(comma3+1);
			return(operate_name + "," + acc_tech);
			*/
			return(req.substring(req.indexOf(F("\""))));
		}
		if(time_out(3000))
		{
			return(F(""));
		}

	}
	return(F(""));
}
unsigned char UC20::SignalQuality()
{
	unsigned char ret = 101;
	_Serial->println(F("AT+CSQ"));
	while(!_Serial->available())
	{}
	start_time_out();
	while(1)
	{
		String req = _Serial->readStringUntil('\n');
		if(req.indexOf(F("+CSQ")) != -1)
		{
			ret = req.substring(req.indexOf(F(" "))+1,req.indexOf(F(","))).toInt();
			return(ret);
		}
		if(time_out(3000))
		{
			return(ret);
		}

	}
	return(ret);
}
void UC20::start_time_out()
{
	previousMillis_timeout = millis();
}
bool UC20::time_out(long timeout_interval)
{
	unsigned long currentMillis = millis();
	if(currentMillis - previousMillis_timeout >= timeout_interval)
	{
		previousMillis_timeout = currentMillis;
		return(true);
	}
	return(false);
}
bool UC20:: wait_ok(long time)
{
	wait_ok_(time,true);
}
bool UC20:: wait_ok_ndb(long time)
{
	wait_ok_(time,false);
}

bool UC20:: wait_ok_(long time,bool ack)
{
	while(!_Serial->available())
	{}
	start_time_out();
	while(1)
	{
		String req = _Serial->readStringUntil('\n');
		if(req.indexOf(F("OK")) != -1)
		{
			if(ack)
			debug(F("OK"));
			return(1);
		}
		if(req.indexOf(F("ERROR")) != -1)
		{
			if(ack)
			debug(F("Error"));
			return(0);
		}
		//debug(req);
	}
	if(time_out(time))
	{
		if(ack)
		debug(F("Error"));
		return(0);
	}
}

unsigned char UC20:: event_input()
{
	while(_Serial->available())
	{
		String req = _Serial->readStringUntil('\n');

	    if(req.indexOf(F("RING")) != -1)
		{
			event_type = EVENT_RING;
			return(EVENT_RING);//EVENT_RING
		}
		else if(req.indexOf(F("+CMTI: \"SM\"")) != -1)
		{
			event_type = EVENT_SMS;
			char index = req.indexOf(F(","));
			index_new_SMS =  req.substring(index+1).toInt();

			return(EVENT_SMS);//EVENT_SMS
		}
	}
	event_type = EVENT_NULL;
	return(EVENT_NULL);//EVENT_NULL
}


/* Code Serial */
String UC20:: readStringUntil(char data)
{
	 return _Serial->readStringUntil(data);
}
size_t UC20:: print(String data)
{
     return _Serial->print(data);
}
size_t UC20:: println(String data)
{
     return _Serial->println(data);
}

void UC20:: print(unsigned char data,int type)
{
	_Serial->print(data,type);
}


void UC20:: print(int data,int type)
{
	_Serial->print(data,type);
}
void UC20:: print(unsigned int data,int type)
{
	_Serial->print(data,type);
}
void UC20:: print(long data,int type)
{
	_Serial->print(data,type);
}

size_t UC20:: print(String data,int type)
{
   int i=0;
   while(data[i])
   {
		_Serial->print(data[i],type);
		i++;
   }
   return(i-1);

   // Serial.print(data,type);
	// return _Serial->print(data,type);
}
size_t UC20:: println(String data,int type)
{
	int i=0;
	while(data[i])
	{
		_Serial->print(data[i],type);
		i++;
	}
	_Serial->println("");
    return(i+1);


	//return _Serial->println(data,type);
}

int UC20:: peek()
{
     return _Serial->peek();
}
size_t UC20:: write(uint8_t byte)
{
     return _Serial->write(byte);
}
int UC20:: read()
{
     return _Serial->read();
}
int UC20:: available()
{
     return _Serial->available();
}
void UC20:: flush()
{
     _Serial->flush();
}
