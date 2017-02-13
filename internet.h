#ifndef INTERNET_h
#define INTERNET_h

#include "TEE_UC20.h"


class INTERNET
{
public:
	INTERNET();
	bool Configure(String apn,String user,String password);
	bool Connect();
	bool DisConnect();
	String GetIP();

};


















#endif 