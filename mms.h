#ifndef MMS_h
#define MMS_h

#include "TEE_UC20.h"


class MMS
{
public:
	MMS();
	bool SetMMSC(String data);
	bool SetProxy(String ip,String port);
	bool Title(String title);
	bool SendTo(String receive);
	bool AddFile(String pattern,String Filename);
	String Send();
	bool Clear();
	String ListMMSFile();
};

#endif
