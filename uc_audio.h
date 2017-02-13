#ifndef UCAUDIO_h
#define UCAUDIO

#include "TEE_UC20.h"
#define UPLINK false
#define DOWNLINK true

class UCxAUDIO
{
public:
	UCxAUDIO();
	bool StartRecord(bool ctrl,String pattern,String fn,unsigned char format,bool dlink);
	bool StartWAVRecord(String pattern,String fn,bool dlink);
	bool StopRecord();
	bool PlayWAV(bool state,String pattern,String fn,bool repeat,bool ulmute,bool dlmute);
	bool PlayWAV(String pattern,String fn,bool ulmute,bool dlmute);
	bool StopWAV();
	void taskAudio();
	bool StatusPlay();
	bool PlayMP3(String pattern,String fn,bool repeat,unsigned char volumn);
	bool StopMP3();
	bool TextToSpeech(String text);
private:
	bool readinput();

};

#endif