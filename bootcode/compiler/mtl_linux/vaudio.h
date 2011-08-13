// VLISP Virtual Machine - 2006 - by Sylvain Huet
// Lowcost IS Powerfull

#ifndef _VAUDIO_
#define _VAUDIO_

#define AUDIO_FIFOPLAY	4096
extern char audioFifoPlay[AUDIO_FIFOPLAY];

void audioInit();

void audioPlayStart(int freq,int bps,int stereo,int trytofeed);
int audioPlayFeed(char *src,int len);
void audioPlayStop();
int audioPlayTryFeed(int ask);
int audioPlayFetchByte();
int audioPlayFetch(char* dst,int ask);

void audioVol(int vol);
int audioPlayTime();


int audioRecStart(int freq,int gain);
int audioRecStop();
int audioRecVol(unsigned char* src,int len,int start);
char* audioRecFeed_begin(int size);
void audioRecFeed_end();

void adpcmdecode(unsigned char* src,char *dstc);
void adpcmencode(short* src,char *dst);

void AudioAdp2wav(char* dst,int idst,int ldst,char *src,int isrc,int lsrc,int len);
void AudioWav2adp(char* dst,int idst,int ldst,char *src,int isrc,int lsrc,int len);
void AudioWav2alaw(char* dst,int idst,int ldst,char *src,int isrc,int lsrc,int len,int mu);
void AudioAlaw2wav(char* dst,int idst,int ldst,char *src,int isrc,int lsrc,int len,int mu);



void audioWrite(int reg,int val);
int audioRead(int reg);
int audioFeed(char *src,int len);
void audioRefresh();
void audioAmpli(int on);

#endif

