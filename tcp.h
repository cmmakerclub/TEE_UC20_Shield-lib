#ifndef TCP_h
#define TCP_h

#include "TEE_UC20.h"


class TCP
{
public:
	TCP();
	bool Open(unsigned char contexid,unsigned char connectid,String service_type,String ip_url,String remote_port,String local_port,unsigned char access_mode);
	bool Open(String ip_url,String port);
	bool StartSend(unsigned char contexid);
	bool StartSend(unsigned char contexid,int len);
	bool StartSend();
	bool StopSend();
	bool WaitSendFinish();
	bool ReceiveAvailable();
	int  ReadBuffer();
	int  ReadBuffer(unsigned char contexid,int max_len);
	int  ReadBuffer(int max_len);
	void Ping(unsigned char contextid,String ip_url);
	void write(char data);
	void print(String data);
	void println(String data);
	void print(int data);
	void println(int data);
	bool Close(unsigned char contexid);
	bool Close();
	bool CheckConnection(unsigned char query_type , unsigned char contexid );
	bool CheckConnection();
	String NTP(unsigned char contexid,String ip_url,String port);
	
	unsigned char ReceiveConnectID;
	
};

#endif