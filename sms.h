#ifndef UC20_SMS
#define UC20_SMS
#include "TEE_UC20.h"


class SMS
{
public:
	SMS();
	void DefaultSetting();
	void Start(String rx_number);
	void Send(String data);
	void Sendln(String data);
	void Stop();
	unsigned char IndexNewSMS();
	String ReadSMS(int index);
	String SMSInfo;
	String ConvertStrUnicodeToTIS620(String data);
	String ConvertStrUnicodeToUTF8(String data);
	bool DeleteSMS(int index);
	
};





#endif