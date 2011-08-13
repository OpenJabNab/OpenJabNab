// Librairie Sound

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include <commctrl.h>
#include <commdlg.h>
#include <mmsystem.h>

#include "mp3/mpg123.h"
#include "mp3/mpglib.h"


#include "simuaudio.h"
#include "vaudio.h"

HWAVEOUT hWaveOut;
HWAVEIN hWaveIn;

int nPlayBuffers;
WaveOutBuffer* WaveOutBuf;
int PlayState;
int PlayStateEof;
int PlayType;

int nRecBuffers;
WaveInBuffer* WaveInBuf;
int SendInBuffers;
BOOL StopRecord;
BOOL RecordInitialised;

#define PLAYST_IDLE 0
#define PLAYST_START 1
#define PLAYST_PLAY 2

#define PLAYTYPE_MP3 0
#define PLAYTYPE_WAV 1
#define PLAYTYPE_ADPCM 2

struct mpstr mp;



#define BUFMP3OUT_LENGTH 65536
#define BUFMP3IN_LENGTH 65536

char bufplayout[BUFMP3OUT_LENGTH];
int bufplayout_i;
char bufplayin[BUFMP3IN_LENGTH];
int bufplaystart;

void mp3simuinit()
{
	InitMP3(&mp);
	bufplayout_i=0;
	bufplaystart=0;
}

char bufmp3[4608];

// mono :  a b -> aaaa bbbb
// stereo : a b -> abababab

void mp3scale(short *src,int len,short *dst,int freq,int lsf,int stereo,int dstlen)
{
	if (stereo==2)
	{
		if (len==dstlen) memcpy((char*)dst,(char*)src,len);
	}
	if (stereo==1)
	{
		int i;
//		int k=dstlen/len;
		dstlen/=2;
		len/=2;
		for(i=0;i<len;i++)
		{
			int j;
			int k=(i+1)*dstlen/len;
			for(j=i*dstlen/len;j<k;j++) *(dst++)=*src;
			src++;
		}
	}
}

void wavscale(short *src,int len,short *dst)
{
	int i;
	len/=2;
	for(i=0;i<len;i++)
	{
		int j;
		for(j=0;j<10;j++) *(dst++)=*src;
		src++;
	}
}

int getmp3len(int freq)
{
	if (freq==16000) return 4608*44100/32000;
	return 4608;
}

void simuFetchMP3(int ask)
{
	int dstlen;
	int len;
	int ret;
	int loop=1;

	while((loop)&&(bufplayout_i<ask))
	{
		ret = decodeMP3(&mp,NULL,0,bufmp3,4608,&len);
		if (ret==MP3_OK)
		{

//			printf("freq=%d lsf=%d stereo=%d -> %d\n",freqs[mp.fr.sampling_frequency],mp.fr.lsf,mp.fr.stereo,len);
			dstlen=getmp3len(freqs[mp.fr.sampling_frequency]);
			mp3scale((short*)bufmp3,len,bufplayout+bufplayout_i,freqs[mp.fr.sampling_frequency],mp.fr.lsf,mp.fr.stereo,dstlen);
			bufplayout_i+=dstlen;
		}
		else
		{
			len=audioPlayFetch(bufplayin+bufplaystart,BUFMP3IN_LENGTH-bufplaystart);
			len+=bufplaystart;
//	printf("simuFetchWav get=%d\n",len);
			if (len)
			{
				bufplaystart=0;
				ret = decodeMP3(&mp,bufplayin,len,bufmp3,4608,&len);
				if (ret==MP3_OK)
				{
//					printf("freq=%d lsf=%d stereo=%d -> %d\n",freqs[mp.fr.sampling_frequency],mp.fr.lsf,mp.fr.stereo,len);
					dstlen=getmp3len(freqs[mp.fr.sampling_frequency]);
					mp3scale(bufmp3,len,bufplayout+bufplayout_i,freqs[mp.fr.sampling_frequency],mp.fr.lsf,mp.fr.stereo,dstlen);
					bufplayout_i+=dstlen;
				}
			}
			else loop=0;
		}
	};
}

void simuFetchWav(int ask)
{
	int len;
	int loop=1;
	int askwav=2+(((ask-bufplayout_i)/10)&(-2));

	while((loop)&&(bufplaystart<askwav))
	{
		len=audioPlayFetch(bufplayin+bufplaystart,askwav-bufplaystart);
	printf("simuFetchWav get=%d\n",len);
		if (len) bufplaystart+=len;
		else loop=0;
	}

	len=bufplaystart&(-2);
	if (len>=askwav) len=askwav;

	wavscale(bufplayin,len,bufplayout+bufplayout_i);
	bufplayout_i+=len*10;
	if (bufplaystart!=len)
	{
		memcpy(bufplayin,bufplayin+len,bufplaystart-len);
		bufplaystart-=len;
	}
	else bufplaystart=0;
}

void simuFetchAdpcm(int ask)
{
	printf("simuFetchAdpcm %d\n",ask);
	while(bufplayout_i<ask)
	{
		if (bufplaystart<256) bufplaystart+=audioPlayFetch(bufplayin+bufplaystart,256-bufplaystart);
		if (bufplaystart<256) return;

		adpcmdecode(bufplayin,bufmp3);
		wavscale(bufmp3,1010,bufplayout+bufplayout_i);
		bufplayout_i+=1010*10;
		bufplaystart-=256;
		memcpy(bufplayin,bufplayin+256,bufplaystart);
	}
}

int simuFetchPlay(char* dst,int ask)
{
//	printf("simuFetchPlay %d\n",PlayType);

	if (PlayType==PLAYTYPE_MP3)simuFetchMP3(ask);
	if (PlayType==PLAYTYPE_WAV)simuFetchWav(ask);
	if (PlayType==PLAYTYPE_ADPCM)simuFetchAdpcm(ask);
//	printf("bufplayout_i=%d\n",bufplayout_i);
	if (bufplayout_i<ask) ask=bufplayout_i;
	if ((PlayStateEof)&&(!bufplayout_i)) PlayState=PLAYST_IDLE;
	if (!ask) return 0;

	memcpy(dst,bufplayout,ask);
	memcpy(bufplayout,bufplayout+ask,bufplayout_i-ask);
	bufplayout_i-=ask;
	return ask;
}

void simuFetchStart()
{
	while(1)
	{
		int len=BUFMP3IN_LENGTH-bufplaystart;
		if (len) len=audioPlayFetch(bufplayin+bufplaystart,len);
	printf("simuFetchStart get=%d\n",len);

		if (!len)
		{
			if (PlayStateEof) PlayState=PLAYST_IDLE;
			return;
		}
		bufplaystart+=len;

		if (1)//(((bufplaystart>=2)&&(bufplayin[0]=='I')&&(bufplayin[1]=='D')&&(bufplayin[2]=='3'))	||((bufplayin[0]&255)==255))
		{
			int i=0;
			int head=0;
			PlayType=PLAYTYPE_MP3;
			printf("detect MP3\n");
			PlayState=PLAYST_PLAY;
			return;

/*			while((i<bufplaystart)&&(!checkmp3header(head)))
			{
				head=(head<<8)+(bufplayin[i++]&255);
			}
			if (i<bufplaystart)
			{
				int j;
				printf("detect start at %d\n",i);
				i-=4;
				len=bufplaystart-i;
				for(j=0;j<len;j++) bufplayin[j]=bufplayin[j+i];
				bufplaystart=len;
				PlayState=PLAYST_PLAY;
				return;
			}
*/		}
		else if ((bufplaystart>20)&&(bufplayin[0]=='R')&&(bufplayin[1]=='I')&&(bufplayin[2]=='F'))
		{
			int i;
			printf("detect wav bufplaystart=%d\n",bufplaystart);
			PlayType=(bufplayin[20]==0x11)?PLAYTYPE_ADPCM:PLAYTYPE_WAV;
			for(i=0;i<bufplaystart-8;i++) if (!strncmp(bufplayin+i,"data",4))
			{
				int j;
				i+=8;
				len=bufplaystart-i;
				for(j=0;j<len;j++) bufplayin[j]=bufplayin[j+i];
				bufplaystart=len;
				PlayState=PLAYST_PLAY;
				return;
			}
		}
	}
}

int simuFetch(char* dst,int ask)
{
//	printf("simuFetch %d\n",PlayState);
	if (PlayState==PLAYST_IDLE) return 0;
	if (PlayState==PLAYST_START) simuFetchStart();
	if (PlayState==PLAYST_PLAY) return simuFetchPlay(dst,ask);
	return 0;
}


extern HWND mainwin;

int OutFillBuffer(int i)
{
	int k,len;

	if (WaveOutBuf[i].lpData==NULL) return 0;
	k=simuFetch(WaveOutBuf[i].lpData,WaveOutBuf[i].dwDataSize);
//	printf("#outfill\n");
	len=WaveOutBuf[i].dwDataSize-k;
	if (len>0) memset(WaveOutBuf[i].lpData+k,0,len);

	return 0;
}
int InFillBuffer(int i)
{
  char *p;

  if (WaveInBuf[i].lpData==NULL) return 0;

	p=audioRecFeed_begin(256);
	adpcmencode(WaveInBuf[i].lpData,p);
/*	p=audioRecFeed_begin(WaveInBuf[i].dwDataSize);
	memcpy(p,WaveInBuf[i].lpData,WaveInBuf[i].dwDataSize);
*/	audioRecFeed_end();

  return 0;
}

void WaveOutFreeAll()
{
  int i;
  for (i=0;i<nPlayBuffers;i++)
  {
    waveOutUnprepareHeader(hWaveOut,WaveOutBuf[i].lpWaveHdr,sizeof(WAVEHDR));

    if (WaveOutBuf[i].lpData != NULL)
        GlobalUnlock(WaveOutBuf[i].hData);
    if (WaveOutBuf[i].hData) 
      GlobalFree(WaveOutBuf[i].hData);

    if (WaveOutBuf[i].lpWaveHdr != NULL)
      GlobalUnlock(WaveOutBuf[i].hWaveHdr);
    if (WaveOutBuf[i].hWaveHdr)
      GlobalFree(WaveOutBuf[i].hWaveHdr);
  }
  memset(WaveOutBuf,0,nPlayBuffers*sizeof(WaveOutBuffer));
}
void WaveInFreeAll()
{
  int i;
  for (i=0;i<nRecBuffers;i++)
  {
    waveInUnprepareHeader(hWaveIn,WaveInBuf[i].lpWaveHdr,sizeof(WAVEHDR));

    if (WaveInBuf[i].lpData != NULL)
        GlobalUnlock(WaveInBuf[i].hData);
    if (WaveInBuf[i].hData) 
      GlobalFree(WaveInBuf[i].hData);

    if (WaveInBuf[i].lpWaveHdr != NULL)
      GlobalUnlock(WaveInBuf[i].hWaveHdr);
    if (WaveInBuf[i].hWaveHdr)
      GlobalFree(WaveInBuf[i].hWaveHdr);
  }
  memset(WaveInBuf,0,nRecBuffers*sizeof(WaveInBuffer));
  RecordInitialised=FALSE;
  SendInBuffers=0;
}


int PlayInit(int rate,int ChannelSize,int nbBuffers)
{
  UINT wResult;
  PCMWAVEFORMAT pcmWaveFormat;
  int i;

  // Open a waveform device for output using window callback.
  int nChannels=2;
  int BitsPerSample=16;

  pcmWaveFormat.wf.wFormatTag=WAVE_FORMAT_PCM;
  pcmWaveFormat.wf.nChannels=nChannels;
  pcmWaveFormat.wf.nSamplesPerSec=rate;
  pcmWaveFormat.wf.nAvgBytesPerSec=rate*nChannels*(BitsPerSample/8);
  pcmWaveFormat.wf.nBlockAlign=(nChannels*BitsPerSample)/8;
  pcmWaveFormat.wBitsPerSample=BitsPerSample;

  wResult=waveOutOpen((LPHWAVEOUT)&hWaveOut, WAVE_MAPPER,
                  (LPWAVEFORMATEX)&pcmWaveFormat,
                  (LONG)mainwin, 0L, CALLBACK_WINDOW);
  if (wResult != 0)	return 0;

  nPlayBuffers=nbBuffers;
  if (WaveOutBuf) free(WaveOutBuf);
  WaveOutBuf=(WaveOutBuffer*)malloc(sizeof(WaveOutBuffer)*nPlayBuffers);


  for (i=0;i<nPlayBuffers;i++)
  {
    WaveOutBuf[i].nChannels=nChannels;
    WaveOutBuf[i].Rate=rate;
    WaveOutBuf[i].Resolution=BitsPerSample;
    WaveOutBuf[i].hwnd=mainwin;
    WaveOutBuf[i].dwDataSize=ChannelSize;
    
    // Allocate and lock memory for the waveform data.
    WaveOutBuf[i].hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, WaveOutBuf[i].dwDataSize );
    if (!WaveOutBuf[i].hData) return 0;
    if ((WaveOutBuf[i].lpData = (char*)GlobalLock(WaveOutBuf[i].hData)) == NULL) {
        WaveOutFreeAll();
        return 0;
    }

    // Allocate and lock memory for the header.
    WaveOutBuf[i].hWaveHdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,(DWORD) sizeof(WAVEHDR));
    if (WaveOutBuf[i].hWaveHdr == NULL)
    {
        WaveOutFreeAll();
        return 0;
    }

    WaveOutBuf[i].lpWaveHdr = (LPWAVEHDR) GlobalLock(WaveOutBuf[i].hWaveHdr);
    if (WaveOutBuf[i].lpWaveHdr == NULL)
    {
        WaveOutFreeAll();
        return 0;
    }

    // After allocation, set up and prepare header.

    WaveOutBuf[i].lpWaveHdr->lpData = WaveOutBuf[i].lpData;
    WaveOutBuf[i].lpWaveHdr->dwBufferLength = WaveOutBuf[i].dwDataSize;
    WaveOutBuf[i].lpWaveHdr->dwUser = i;
    WaveOutBuf[i].lpWaveHdr->dwFlags = 0L;
    WaveOutBuf[i].lpWaveHdr->dwLoops = 1L;
    wResult = waveOutPrepareHeader(hWaveOut,WaveOutBuf[i].lpWaveHdr,sizeof(WAVEHDR));
    if (wResult != 0)
    {
      wResult=waveOutClose(hWaveOut);
      WaveOutFreeAll();
      return 0;
    }
	memset(WaveOutBuf[i].lpData,0,WaveOutBuf[i].dwDataSize);
  }

  for (i=0;i<nPlayBuffers;i++)
  {
    // Now the data block can be sent to the output device. The
    // waveOutWrite function returns immediately and waveform
    // data is sent to the output device in the background.
    wResult = waveOutWrite(hWaveOut,WaveOutBuf[i].lpWaveHdr,sizeof(WAVEHDR));
    if (wResult != 0)
    {
      wResult=waveOutReset(hWaveOut);
      wResult=waveOutClose(hWaveOut);
      WaveOutFreeAll();
      return 0;
    }
  }
  return 1;
}

int RecStart(int rate,int ChannelSize,int nbBuffers)
{
  UINT wResult;
  PCMWAVEFORMAT pcmWaveFormat;
  int r,i;
  int BitsPerSample=16;
  int nChannels=1;
  if (RecordInitialised) return 0;
  nRecBuffers=nbBuffers;
  if (WaveInBuf) free(WaveInBuf);
  WaveInBuf=(WaveInBuffer*)malloc(sizeof(WaveInBuffer)*nRecBuffers);
//  memset(WaveInBuf,0,nRecBuffers*sizeof(WaveInBuffer));
  // Open a waveform device for output using window callback.

  if (BitsPerSample>8) BitsPerSample=16;
  pcmWaveFormat.wf.wFormatTag=WAVE_FORMAT_PCM;
  pcmWaveFormat.wf.nChannels=nChannels;
  pcmWaveFormat.wf.nSamplesPerSec=rate;
  pcmWaveFormat.wf.nAvgBytesPerSec=rate*nChannels*(BitsPerSample/8);
  pcmWaveFormat.wf.nBlockAlign=(nChannels*BitsPerSample)/8;
  pcmWaveFormat.wBitsPerSample=BitsPerSample;

  if (r=waveInOpen((LPHWAVEIN)&hWaveIn, WAVE_MAPPER,
                  (LPWAVEFORMATEX)&pcmWaveFormat,
                  (LONG)mainwin, 0L, CALLBACK_WINDOW))
  {
//      MMechostr(MSKDEBUG,"Record: Unable to Open Device\n");
      switch (r)
      {
        case MMSYSERR_ALLOCATED : printf("Device allready allocated\n"); break;
        case MMSYSERR_BADDEVICEID : printf("Bad device ID\n"); break;
        case MMSYSERR_NODRIVER : printf("No driver\n"); break;
        case WAVERR_BADFORMAT : printf("Bad Format\n"); break;
        case MMSYSERR_NOMEM : printf("No memory\n"); break;
      }
      return 0;
  }

  for (i=0;i<nRecBuffers;i++)
  {
    WaveInBuf[i].nChannels=nChannels;
    WaveInBuf[i].Rate=rate;
    WaveInBuf[i].Resolution=BitsPerSample;
    WaveInBuf[i].dwDataSize=nChannels*(BitsPerSample/8)*ChannelSize;
    WaveInBuf[i].hwnd=mainwin;
    // Allocate and lock memory for the waveform data.
    WaveInBuf[i].hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, WaveInBuf[i].dwDataSize );
    if (!WaveInBuf[i].hData)
    {
//        MMechostr(MSKDEBUG,"Record: Unable to Allocate memory\n");
        return 0;
    }
    if ((WaveInBuf[i].lpData = (char*)GlobalLock(WaveInBuf[i].hData)) == NULL) {
//        MMechostr(MSKDEBUG,"Record: Unable to Lock Memory\n");
        WaveInFreeAll();
        return 0;
    }

    // Allocate and lock memory for the header.
    WaveInBuf[i].hWaveHdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
        (DWORD) sizeof(WAVEHDR));
    if (WaveInBuf[i].hWaveHdr == NULL)
    {
        WaveInFreeAll();
//        MMechostr(MSKDEBUG,"Record: Unable to allocate wave header\n");
        return 0;
    }

    WaveInBuf[i].lpWaveHdr = (LPWAVEHDR) GlobalLock(WaveInBuf[i].hWaveHdr);
    if (WaveInBuf[i].lpWaveHdr == NULL)
    {
        WaveInFreeAll();
//        MMechostr(MSKDEBUG,"Record: Unable to Lock wave header memory\n");
        return 0;
    }

    // After allocation, set up and prepare header.

    WaveInBuf[i].lpWaveHdr->lpData = WaveInBuf[i].lpData;
    WaveInBuf[i].lpWaveHdr->dwBufferLength = WaveInBuf[i].dwDataSize;
    WaveInBuf[i].lpWaveHdr->dwUser = i;
    WaveInBuf[i].lpWaveHdr->dwFlags = 0L;
    WaveInBuf[i].lpWaveHdr->dwLoops = 0L;
    wResult=waveInPrepareHeader(hWaveIn,WaveInBuf[i].lpWaveHdr,sizeof(WAVEHDR));
    if (wResult != 0)
    {
      char buf[MAXERRORLENGTH];
      WaveInFreeAll();
//      MMechostr(MSKDEBUG,"PrepareRecBuf: Unable to write block to the device\n");
      waveOutGetErrorText(wResult,buf,MAXERRORLENGTH);
//      MMechostr(MSKDEBUG,buf);
//      MMechostr(MSKDEBUG,"\n");
      return 0;
    }

  }
    
  for (i=0;i<nRecBuffers;i++)
  {
    // Now the data block can be sent to the input device. The
    // waveOutWrite function returns immediately and waveform
    // data is sent to the output device in the background.
    wResult = waveInAddBuffer(hWaveIn,WaveInBuf[i].lpWaveHdr,sizeof(WAVEHDR));
    if (wResult != 0)
    {
        WaveInFreeAll();
//        MMechostr(MSKDEBUG,"Record: Unable to write block to the device\n");
        return 0;
    }
    SendInBuffers++;
  }
  waveInStart(hWaveIn);
  StopRecord=FALSE;
  RecordInitialised=TRUE;

  return 1;
}

int PlayStart()
{
	PlayState=PLAYST_START;
	PlayStateEof=0;
	mp3simuinit();
	return 0;
}

int PlayStop()
{
	PlayState=PLAYST_IDLE;
	return 0;
}

int PlayEof()
{
	PlayStateEof=1;
	return 0;
}

int RecStop()
{
//  waveInReset(hWaveIn);
//  waveInClose(hWaveIn);
	StopRecord=TRUE;
	return 0;
}

int PlayData(int i)
{
  WORD wResult;

    OutFillBuffer(i);
    wResult = waveOutWrite(hWaveOut,WaveOutBuf[i].lpWaveHdr,sizeof(WAVEHDR));
    if (wResult != 0)
    {
      WaveOutFreeAll();
      return 0;
    }
  return 1;
}

int RecData(int i)
{
  WORD wResult;

  SendInBuffers--;
  if (!StopRecord)
  {
    InFillBuffer(i);
    wResult = waveInAddBuffer(hWaveIn,WaveInBuf[i].lpWaveHdr,sizeof(WAVEHDR));
    if (wResult != 0)
    {
      WaveInFreeAll();
      return 0;
    }
    SendInBuffers++;
  }
  else
  {
    if (!SendInBuffers)
    {
      WaveInFreeAll();
      StopRecord=FALSE;
	  waveInClose(hWaveIn);
    }
  }
  return 1;
}

int audioEventPlayData(int eventId, int wParam, int lParam)
{
//	printf("###play cb\n");
	PlayData(((LPWAVEHDR)lParam)->dwUser);
	return 0;
}

int audioEventRecData(int eventId, int wParam, int lParam)
{
//	printf("###rec cb\n");
	RecData(((LPWAVEHDR)lParam)->dwUser);
	return 0;
}

void audioSetVolume(int vol)
{
	vol&=255;
	waveOutSetVolume(hWaveOut,(vol<<24)+(vol<<8));
}

int simuaudioinit()
{
	nPlayBuffers=0;
	PlayState=PLAYST_IDLE;
	
	nRecBuffers=0;
	SendInBuffers=0;
	StopRecord=FALSE;
	RecordInitialised=FALSE;

	PlayInit(44100,10000 //22050
		,8);

	return 0;
}

