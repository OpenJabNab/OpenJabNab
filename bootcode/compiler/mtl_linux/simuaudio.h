//-------------------
// MV
// version WIN32
// Sylvain Huet
// Premiere version : 07/01/2003
// Derniere mise a jour : 07/01/2003
//

#ifndef _AUDIO_
#define _AUDIO_


// Global variables & type defs
typedef struct WaveOutBuffer
{
  WORD Rate;
  BYTE nChannels;
  LONG dwDataSize;
  BYTE Resolution;
  HWND hwnd;
  HGLOBAL hWaveHdr;
  LPWAVEHDR lpWaveHdr;
  HANDLE hData;  // handle of waveform data memory 
  HPSTR  lpData;  // pointer to waveform data memory 
} WaveOutBuffer;

typedef struct WaveInBuffer
{
  WORD Rate;
  BYTE nChannels;
  LONG dwDataSize;
  BYTE Resolution;
  HWND hwnd;
  HGLOBAL hWaveHdr;
  LPWAVEHDR lpWaveHdr;
  HANDLE hData;  // handle of waveform data memory 
  HPSTR  lpData;  // pointer to waveform data memory 
} WaveInBuffer;

// audio
extern HWAVEOUT hWaveOut;
extern HWAVEIN hWaveIn;

extern int nPlayBuffers;
extern WaveOutBuffer* WaveOutBuf;
extern int SendOutBuffers;
extern BOOL StopPlayback;
extern BOOL PlaybackInitialised;

extern int nRecBuffers;
extern WaveInBuffer* WaveInBuf;
extern int SendInBuffers;
extern BOOL StopRecord;
extern BOOL RecordInitialised;

int PlayStart();
int PlayStop();
int PlayEof();

int RecStart(int rate,int ChannelSize,int nbBuffers);
int RecStop();
int RecData(int i);

int audioEventPlayData(int eventId, int wParam, int lParam);
int audioEventRecData(int eventId, int wParam, int lParam);
void audioSetVolume(int vol);
int simuaudioinit();


void mp3simuinit();
int mp3simuFetch(char* dst,int ask);

#endif

