#include "uc_mqtt.h"
#include "tcp.h"
TCP tcp_mqtt;
bool connected = false;
unsigned int len_buffer_in_module = 0;
void func_null(String topic ,char *playload,unsigned char length){}

const long interval_ping = 10000; 
unsigned long previousMillis_ping; 
unsigned long currentMillis_ping;  
	
UCxMQTT::UCxMQTT()
{
	//void (*callback)(String topic ,char *playload,unsigned char length);
	callback = func_null;
	
}
bool UCxMQTT::ConnectMQTTServer(String web , String port)
{
	//connected = tcp_mqtt.Open(1,0,"TCP",web,port,"0",1); //direct mode
	connected = tcp_mqtt.Open(1,0,"TCP",web,port,"0",0);//buffer mode	
	return (connected); 
}

bool UCxMQTT::DisconnectMQTTServer()
{
	connected = false;
	return (tcp_mqtt.Close()); 
}

unsigned char UCxMQTT::Connect(const char *id, const char *user, const char *pass, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage) 
{
		    connected = false;
			nextMsgId = 1;
            // Leave room in the buffer for header and variable length field
            uint16_t length = 5;
            unsigned int j;

#if MQTT_VERSION == MQTT_VERSION_3_1
            uint8_t d[9] = {0x00,0x06,'M','Q','I','s','d','p', MQTT_VERSION};
#define MQTT_HEADER_VERSION_LENGTH 9
#elif MQTT_VERSION == MQTT_VERSION_3_1_1
            uint8_t d[7] = {0x00,0x04,'M','Q','T','T',MQTT_VERSION};
#define MQTT_HEADER_VERSION_LENGTH 7
#endif
            for (j = 0;j<MQTT_HEADER_VERSION_LENGTH;j++) {
                buffer[length++] = d[j];
            }

            uint8_t v;
            if (willTopic) {
                v = 0x06|(willQos<<3)|(willRetain<<5);
            } else {
                v = 0x02;
            }

            if(user != NULL) {
                v = v|0x80;

                if(pass != NULL) {
                    v = v|(0x80>>1);
                }
            }

            buffer[length++] = v;

            buffer[length++] = ((MQTT_KEEPALIVE) >> 8);
            buffer[length++] = ((MQTT_KEEPALIVE) & 0xFF);
            length = writeString(id,buffer,length);
            if (willTopic) {
                length = writeString(willTopic,buffer,length);
                length = writeString(willMessage,buffer,length);
            }

            if(user != NULL) {
                length = writeString(user,buffer,length);
                if(pass != NULL) {
                    length = writeString(pass,buffer,length);
                }
            }
		write(MQTTCONNECT,buffer,length-5);
		//int ret = red_data_from_3G();
		int ret = red_data_from_3G_buffermode();
		
		
		if(ret==-1)
			connected = false;
		else if(buffer[ret]==0)
			connected = true;
		else
			connected = false;
		//Serial.println(ret);
		return(buffer[ret]);
}
unsigned char UCxMQTT::Connect(const char *id, const char *user, const char *pass) 
{
	return(Connect(id,user,pass,0,0,0,0));
}
int UCxMQTT::red_data_from_3G_buffermode()
{
	int ret = 99;
	unsigned long pv_timeout   = millis();;
	const long interval_timeout = 5000;
	bool state_data_in=false;
	//Serial.println();
	//Serial.println("testbuf");
	//Serial.println();
	
	while(1)
	{
		unsigned long current_timeout = millis();
		ret=0;
		if(tcp_mqtt.ReceiveAvailable())
		{
			int len = tcp_mqtt.ReadBuffer(10);
			Serial.println("len = ");
			Serial.println(len);
			while(len)
			{
				if(gsm.available())
				{
					char c = gsm.read();
					buffer[ret] = c;
					ret++;
					//Serial.print(c,HEX);
					len--;
				}
			}
			if(buffer[0]=0x20)
			{
				ret-=1;
				//Serial.println(ret);
				return(ret);
			}
		}
		if(current_timeout - pv_timeout >= interval_timeout)
		{
			return(-1);
		}
	}
	
}

uint16_t UCxMQTT::writeString(const char* string, uint8_t* buf, uint16_t pos) {
    const char* idp = string;
    uint16_t i = 0;
    pos += 2;
    while (*idp) {
        buf[pos++] = *idp++;
        i++;
    }
    buf[pos-i-2] = (i >> 8);
    buf[pos-i-1] = (i & 0xFF);
    return pos;
}

bool UCxMQTT::write(uint8_t header, uint8_t* buf, uint16_t length) {
    uint8_t lenBuf[4];
    uint8_t llen = 0;
    uint8_t digit;
    uint8_t pos = 0;
    uint16_t rc;
    uint16_t len = length;
    do {
        digit = len % 128;
        len = len / 128;
        if (len > 0) {
            digit |= 0x80;
        }
        lenBuf[pos++] = digit;
        llen++;
    } while(len>0);

    buf[4-llen] = header;
    for (int i=0;i<llen;i++) {
        buf[5-llen+i] = lenBuf[i];
    }

#ifdef MQTT_MAX_TRANSFER_SIZE
    uint8_t* writeBuf = buf+(4-llen);
    uint16_t bytesRemaining = length+1+llen;  //Match the length type
    uint8_t bytesToWrite;
    boolean result = true;
    while((bytesRemaining > 0) && result) 
	{
        bytesToWrite = (bytesRemaining > MQTT_MAX_TRANSFER_SIZE)?MQTT_MAX_TRANSFER_SIZE:bytesRemaining;
        rc = write_tcp(writeBuf,bytesToWrite);//////////////////////////stk/////
        result = (rc == bytesToWrite);
        bytesRemaining -= rc;
        writeBuf += rc;
    }
    return result;
#else
    rc = write_tcp(buf+(4-llen),length+1+llen);  /////////////////////////////stk///////////
    lastOutActivity = millis();
    return (rc == 1+llen+length);
#endif
}


uint8_t UCxMQTT::write_tcp(uint8_t* buf, uint8_t length) 
{
	uint8_t len_let=0;
	if(tcp_mqtt.StartSend(0,length))
	{
		for(int itcp=0;itcp<length;itcp++)
	   {
			tcp_mqtt.write(buf[itcp]);
			len_let++;
	   }
	   if(!tcp_mqtt.WaitSendFinish())
	   {
		   Serial.println("false unfinish");
		   connected = false;
	   }
	}
	else
	{
		connected = false;
	}
   return(len_let);
}

bool UCxMQTT::Publish(const char* topic, const uint8_t* payload, unsigned int plength, boolean retained)
{
    
        if (MQTT_MAX_PACKET_SIZE < 5 + 2+strlen(topic) + plength) {
            // Too long
            return false;
        }
        // Leave room in the buffer for header and variable length field
        uint16_t length = 5;
        length = writeString(topic,buffer,length);
        uint16_t i;
        for (i=0;i<plength;i++) {
            buffer[length++] = payload[i];
        }
        uint8_t header = MQTTPUBLISH;
        if (retained) {
            header |= 1;
        }
        return write(header,buffer,length-5);

}
bool UCxMQTT::Publish(String topic,String payload, boolean retained)
{
	char data1[100] ;
	topic.toCharArray(data1,topic.length()+1);
	char buf[100];
	payload.toCharArray(buf,payload.length()+1);
	return(Publish(data1,(const uint8_t*)buf,payload.length()+1,retained));//(const uint8_t*)"xxx"
}

bool UCxMQTT::Subscribe(const char* topic) {
    return Subscribe(topic, 0);
}

bool UCxMQTT::Subscribe(const char* topic, uint8_t qos) {
    if (qos < 0 || qos > 1) {
        return false;
    }
    if (MQTT_MAX_PACKET_SIZE < 9 + strlen(topic)) {
        // Too long
        return false;
    }
        // Leave room in the buffer for header and variable length field
        uint16_t length = 5;
        nextMsgId++;
        if (nextMsgId == 0) {
            nextMsgId = 1;
        }
        buffer[length++] = (nextMsgId >> 8);
        buffer[length++] = (nextMsgId & 0xFF);
        length = writeString((char*)topic, buffer,length);
        buffer[length++] = qos;
        return write(MQTTSUBSCRIBE|MQTTQOS1,buffer,length-5);
    return false;
}

String UCxMQTT:: ConnectReturnCode(unsigned char input)
{
	switch(input)
	{
		case 0:
			return(F("Connection Accepted"));
		break;
		case 1:
			return(F("Connection Refused, unacceptable protocol version"));
		break;
		case 2:
			return(F("Connection Refused, identifier rejected"));
		break;
		case 3:
			return(F("Connection Refused, Server unavailable"));
		break;
		case 4:
			return(F("Connection Refused, bad user name or password"));
		break;
		case 5:
			return(F("Connection Refused, not authorized"));
		break;
		default:
			return(F("Unknow!!!"));
		break;
	}
}
void UCxMQTT::Ping()
{
	buffer[0] = 0xC0;
	buffer[1] = 0x00;
	write_tcp(buffer,2);
	//return(connected);
}
void UCxMQTT::MqttLoop()
{
	unsigned char ret;
	currentMillis_ping = millis();
	static bool ping_flag=true;
	if(currentMillis_ping - previousMillis_ping >= interval_ping) 
	{
		if(ping_flag==false)
		{
			connected = false;
		}
		Ping();
		ping_flag = false;
		previousMillis_ping = currentMillis_ping ;
	}
	if(gsm.available())
	{
			String req = gsm.readStringUntil('\r');
			if(req.indexOf(F("+QIURC: \"closed\""))!= -1)
			{
				connected = false;
				return;
			}
				unsigned int buf_cnt=0;
				while(1)
				{
					unsigned int len_in_buffer = ReadDataInBufferMode(1);
					if(len_in_buffer==0)
						return;
					switch (buffer[0])
					{
						case 0x30: // rx_sub
							check_rx_sub();
						break;
						case 0xD0: //ping
							ReadDataInBufferMode(1);
							if(buffer[0]==0x00)
							{
								connected = true;
								ping_flag = true;
								Serial.println("ping ok");
							}
							else
							{
								ping_flag = false;
								connected = false;
							}
								
						break;
					}
					currentMillis_ping = millis();
					if(currentMillis_ping - previousMillis_ping >= interval_ping) 
					{
						Ping();
						previousMillis_ping = currentMillis_ping ;
					}					
				}
	}
}

void UCxMQTT::check_rx_sub()
{
	unsigned int all_byte;
	unsigned char topic_len;
	unsigned char topic_cnt=0;
	unsigned char playload_cnt=0;
	unsigned int i;
	unsigned int len_in_buffer = ReadDataInBufferMode(1);
	all_byte = buffer[0];
	len_in_buffer = ReadDataInBufferMode(2);
	topic_len = buffer[1];
	len_in_buffer = ReadDataInBufferMode(all_byte-2);
	char topic[topic_len+1];
	char playload[len_in_buffer+1];
	for(i=0;i<len_in_buffer;i++)
	{
		if(i<topic_len)
		{
			topic[topic_cnt]=buffer[i];
			topic_cnt++;
		}
		else
		{
			playload[playload_cnt] = buffer[i];
			playload_cnt++;
		}
	}
	topic[topic_cnt]=0;
		String str_topic(topic);
		(*callback)(str_topic,playload,playload_cnt);
}

unsigned int UCxMQTT::ReadDataInBufferMode(unsigned int buf_len)
{
	unsigned int len = tcp_mqtt.ReadBuffer(buf_len);
	unsigned int re_turn = len; 
	unsigned int ret=0;
	
	//Serial.println("\r\ndata");
	while(len)
	{
		if(gsm.available())
		{
			char c = gsm.read();
			//Serial.print(c,HEX);
			buffer[ret] = c;
			ret++;
			//Serial.print(c,HEX);
			len--;
			if(ret > MQTT_MAX_PACKET_SIZE)
				return(ret);
		}
	}
		return(re_turn);
}
bool UCxMQTT::ConnectState()
{
	return(connected);
}













