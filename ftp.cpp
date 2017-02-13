#include "ftp.h"

void list_null(String data){}
void list_null(char data){}

FTP::FTP()
{
	ListOutput =  list_null;
	GetOutput  =  list_null;
}

bool FTP:: begin(unsigned char context_ID)
{
	//AT+QFTPCFG="contextid",1
	gsm.print(F("AT+QFTPCFG=\"contextid\","));
	gsm.print(context_ID,DEC);
	gsm.println("");
	return(gsm.wait_ok(3000));
}
bool FTP::SetUsernamePassword(String user,String pass)
{
	//AT+QFTPCFG="account","User name","Password"
	gsm.print(F("AT+QFTPCFG=\"account\",\""));
	gsm.print(user);
	gsm.print(F("\",\""));
	gsm.print(pass);
	gsm.println(F("\""));
	return(gsm.wait_ok(3000));
}
bool FTP::SetFileType(unsigned char type)
{
	//AT+QFTPCFG="filetype",0 
	gsm.print(F("AT+QFTPCFG=\"filetype\","));
	gsm.print(type,DEC);
	gsm.println("");
	return(gsm.wait_ok(3000));
}
bool FTP::SetTransMode(unsigned char type)
{
	gsm.print(F("AT+QFTPCFG=\"transmode\","));
	gsm.print(type,DEC);
	gsm.println("");
	return(gsm.wait_ok(3000));
}
bool FTP::SetTimeout(int t)
{
	gsm.print(F("AT+QFTPCFG=\"rsptimeout\","));
	gsm.print(t,DEC);
	gsm.println("");
	return(gsm.wait_ok(3000));
}
int FTP:: LoginServer(String serv,int port)
{
	//AT+QFTPOPEN="Your Server URL.com",21
	gsm.print(F("AT+QFTPOPEN=\""));
	gsm.print(serv);
	gsm.print("\",");
	gsm.print(port,DEC);
	gsm.println("");
	while(!gsm.available())
	{}
	gsm.start_time_out();
	
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	    gsm.debug(req);
		if(req.indexOf(F("+QFTPOPEN:")) != -1)
		{
			char index1 = req.indexOf(F(","));
			return(req.substring(index1+1).toInt());
		}
		if(req.indexOf(F("ERROR")) != -1)
		{
			return(-1);
		}		
	}			
	
}
int FTP :: SSLEnable()
{
	int res = 0;
	//AT+QFTPCFG="ssltype",1
	gsm.println(F("AT+QFTPCFG=\"ssltype\",1"));
	res = gsm.wait_ok(3000);
	//AT+QFTPCFG="sslctxid",1   
	gsm.println(F("AT+QFTPCFG=\"sslctxid\",1"));
	res = gsm.wait_ok(3000);
	//AT+QSSLCFG=“ciphersuite”,1, 0xffff 
	gsm.println(F("AT+QSSLCFG=\"ciphersuite\",1, 0xffff"));
	res = gsm.wait_ok(3000);
	//AT+QSSLCFG=“seclevel”,1,0 
	gsm.println(F("AT+QSSLCFG=\"seclevel\",1,0"));
	res = gsm.wait_ok(3000);
	//AT+QSSLCFG=“sslversion”,1,1 
	delay(1000);
	gsm.println(F("AT+QSSLCFG=\"sslversion\",1,1"));  
	res = gsm.wait_ok(3000);
	
}
int FTP :: Logout()
{
	//AT+QFTPCLOSE 
	gsm.println(F("AT+QFTPCLOSE"));
	while(!gsm.available())
	{}
	gsm.start_time_out();
	
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	    gsm.debug(req);
		if(req.indexOf(F("+QFTPCLOSE:")) != -1)
		{
			char index1 = req.indexOf(F(","));
			return(req.substring(index1+1).toInt());
		}
		if(req.indexOf(F("ERROR")) != -1)
		{
			return(-1);
		}		
	}			
}

int  FTP :: SetPath(String path)
{
	gsm.print(F("AT+QFTPCWD=\""));
	gsm.print(path);
	gsm.println(F("\""));
	while(!gsm.available())
	{}
	gsm.start_time_out();
	
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	    gsm.debug(req);
		if(req.indexOf(F("+QFTPCWD:")) != -1)
		{
			char index1 = req.indexOf(F(","));
			return(req.substring(index1+1).toInt());
		}
		if(req.indexOf(F("ERROR")) != -1)
		{
			return(-1);
		}		
	}
}

bool FTP :: List(String path)
{
	//AT+QFTPLIST=“.”
	gsm.print(F("AT+QFTPLIST=\""));
	gsm.print(path);
	gsm.println(F("\",\"COM:\"")); 
	while(!gsm.available())
	{}
	gsm.start_time_out();
	
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
	    //gsm.debug(req);
		(*ListOutput)(req);
		if(req.indexOf(F("CONNECT")) != -1)
		{
			//char index1 = req.indexOf(F(","));
			//return(req.substring(index1+1).toInt());
		}
		if(req.indexOf(F("ERROR")) != -1)
		{
			return(false);
		}
		if(req.indexOf(F("+QFTPLIST:")) != -1)
		{
			return(true);
		}				
	} 	
}
int FTP :: ListToMemory(String path,String pattern,String fn)
{
	gsm.print(F("AT+QFTPLIST=\""));
	gsm.print(path);
	gsm.print(F("\",\""));
	if(pattern == UFS)
	{
		gsm.print(fn);
	}
	if(pattern == RAM)
	{
		gsm.print(F("RAM:"));
		gsm.print(fn);
	}
	gsm.println(F("\""));
	while(!gsm.available())
	{}
	gsm.start_time_out();
	
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
		gsm.debug(req);
		if(req.indexOf(F("+QFTPLIST:")) != -1)
		{
			char index1 = req.indexOf(F(","));
			return(req.substring(index1+1).toInt());
		}	
		if(req.indexOf(F("ERROR")) != -1)
		{
			return(-1);
		}				
	}
	
}
int FTP:: MakeFolder(String name)
{
	//AT+QFTPMKDIR=
	gsm.print(F("AT+QFTPMKDIR=\""));
	gsm.print(name);
	gsm.println(F("\""));
	return(proc_rx_cmd(F("+QFTPMKDIR:")));
}

int FTP::RenameFolder(String name_old,String name_new)
{
	//AT+QFTPRENAME="TEST","TEST_NEW" 
	gsm.print(F("AT+QFTPRENAME=\""));
	gsm.print(name_old);
	gsm.print(F("\",\""));
	gsm.print(name_new);
	gsm.println(F("\""));
	return(proc_rx_cmd(F("+QFTPRENAME:")));
}
int FTP:: DeleteFolder(String name)
{
	//AT+QFTPRMDIR=“TEST_NEW”
	gsm.print(F("AT+QFTPRMDIR=\""));
	gsm.print(name);
	gsm.println(F("\""));
	return(proc_rx_cmd(F("+QFTPRMDIR:")));
}

int FTP:: DeleteFile(String name)
{
	//AT+QFTPRMDIR=“TEST_NEW”
	gsm.print(F("AT+QFTPDEL=\""));
	gsm.print(name);
	gsm.println(F("\""));
	return(proc_rx_cmd(F("+QFTPDEL:")));
}

int FTP:: proc_rx_cmd(String match)
{
	while(!gsm.available())
	{}
	gsm.start_time_out();
	
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
		//gsm.debug(req);
		if(req.indexOf(match) != -1)
		{
			char index1 = req.indexOf(F(","));
			return(req.substring(index1+1).toInt());
		}	
		if(req.indexOf(F("ERROR")) != -1)
		{
			return(-1);
		}				
	}
}
int FTP:: put(String File,String pattern,String fn,int startpos)
{
	put(File,pattern,fn,startpos,0,0xFF);
}


int FTP:: put(String File,String pattern,String fn,int startpos,int uploadlen,int beof)
{
	//AT+QFTPPUT=“test.txt”, “COM:”,0
	
	gsm.print(F("AT+QFTPPUT=\""));
	gsm.print(File);
	gsm.print(F("\",\""));
	
	if(pattern == UFS)
	{
		gsm.print(fn);
	}
	if(pattern == COM)
	{
		gsm.print(F("COM:"));
	}
	if(pattern == RAM)
	{
		gsm.print(F("RAM:"));
		gsm.print(fn);
	}
	gsm.print(F("\","));
	gsm.print(startpos,DEC);
	
	if(uploadlen>0)
	{
		gsm.print(F(","));
		gsm.print(uploadlen,DEC);
	}
	if(beof!=0xFF)
	{
		gsm.print(F(","));
		gsm.print(beof,DEC);
	}
	gsm.println("");
	
	if(pattern == COM)
	{
	  return(proc_rx_cmd2(F("CONNECT")));
	}
	else
	{
		WaitFinish();
	}
}
int FTP:: WaitFinish()
{ 
	return(proc_rx_cmd2(F("+QFTPPUT:")));
}

int FTP:: proc_rx_cmd2(String match)
{
	while(!gsm.available())
	{}
	gsm.start_time_out();
	
	while(1)
	{
		String req = gsm.readStringUntil('\n');	
		//gsm.debug(req);
		if(req.indexOf(match) != -1)
		{
			return(1);
		}	
		if(req.indexOf(F("ERROR")) != -1)
		{
			return(0);
		}				
	}
}
int  FTP::get(String File,int startpos)
{
	 get(File,COM,"",startpos,-1);
}

int  FTP::get(String File,String pattern,String fn)
{
	 get(File,pattern,fn,-1,-1);
}

int FTP:: get(String File,String pattern,String fn,int startpos,int downloadlen)
{
	//AT+QFTPGET=“test.txt”, “COM:”,500,500
	gsm.print(F("AT+QFTPGET=\""));
	gsm.print(File);
	gsm.print(F("\",\""));
	
	if(pattern == UFS)
	{
		gsm.print(fn);
	}
	if(pattern == COM)
	{
		gsm.print(F("COM:"));
	}
	if(pattern == RAM)
	{
		gsm.print(F("RAM:"));
		gsm.print(fn);
	}
	gsm.print(F("\""));
	
	if(startpos!=-1)
	{
		gsm.print(F(","));
		gsm.print(startpos,DEC);
	}
	if(downloadlen!=-1)
	{
		gsm.print(F(","));
		gsm.print(downloadlen,DEC);
	}
	gsm.println("");
	
	if(pattern == COM)
	{
		proc_rx_cmd2(F("CONNECT"));
		Read_get();
	}	
	
	else
	{
		proc_rx_cmd2(F("+QFTPGET"));
	}
		
	/*	
		while(!gsm.available())
		{}
		gsm.start_time_out();
		while(1)
		{
			String req = gsm.readStringUntil('\n');	
			
			if(req.indexOf("+QFTPGET") != -1)
			{
				char index1 = req.indexOf(F(","));
				return(req.substring(index1+1).toInt());
			}	
			else if(req.indexOf(F("OK")) != -1)
			{
				
			}
			else if(req.indexOf(F("ERROR")) != -1)
			{
				return(-1);
			}		
			else
			{
				//gsm.debug(req);
				(*GetOutput)(req);
			}				
		}
		*/
		
}
bool FTP :: WaitFtpFinish()
{
	if(gsm.available())
	{
		String req = gsm.readStringUntil('\n');	
		if(req.indexOf("+QFTPGET") != -1)
		{
				char index1 = req.indexOf(F(" "));
				char index2 = req.indexOf(F(","));
				int resp = req.substring(index1+1,index2).toInt();
				if(resp==0)
				{
					gsm.debug(req);
				}
				return(false);
		}	
	}
	return(true);
}
//wait ด้วยนะ//+QFTPGET: 0,4307419

bool FTP :: Read_get()
{
	unsigned char flag_out=1;
	static char pv =' ';
	while(flag_out)
	{
		while(gsm.available())
		{
			char c = gsm.read();
			//Serial.write(c);
			(*GetOutput)(c);
			if((pv=='O')&&(c=='K'))
			{
				while(flag_out)
				{
					String req = gsm.readStringUntil('\n');	
					//gsm.debug(req);
					//flag_out=0;
					if(req.indexOf(F("+QFTPGET:")) != -1)
					{
						flag_out=0;
					} 
			}
         
		}
		pv = c;
    }
  }
  return(true);
 
}





























