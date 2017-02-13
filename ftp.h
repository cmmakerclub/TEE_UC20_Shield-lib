#ifndef FTP_h
#define FTP_h

#include "TEE_UC20.h"

#define ASCII  0
#define BINARY 1
#define PASSIVE  1
#define ACTIVE   0

class FTP
{
public:
	FTP();
	void (*ListOutput)(String data);
	void (*GetOutput)(char data);
	bool begin(unsigned char context_ID);
	bool SetUsernamePassword(String user,String pass);
	bool SetFileType(unsigned char type);
	bool SetTransMode(unsigned char type);
	bool SetTimeout(int t);
	int  LoginServer(String serv,int port);
	int  SSLEnable();
	int  Logout();
	int  SetPath(String path);
	bool List(String path);
	int  ListToMemory(String path,String pattern,String fn);
	int  MakeFolder(String name);
	int  RenameFolder(String name_old,String name_new);
	int  DeleteFolder(String name);
	int  DeleteFile(String name);
	int  put(String File,String pattern,String fn,int startpos,int uploadlen,int beof);
	int  put(String File,String pattern,String fn,int startpos);
	int  get(String File,String pattern,String fn,int startpos,int downloadlen);
	int  get(String File,int startpos);
	int  get(String File,String pattern,String fn);
	int  WaitFinish();
	bool Read_get();
	bool WaitFtpFinish();
	

private:
	int proc_rx_cmd(String match);
	int proc_rx_cmd2(String match);
};

#endif