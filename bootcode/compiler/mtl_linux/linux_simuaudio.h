#ifndef MY_SIMUAUDIO_H_
# define MY_SIMUAUDIO_H_

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


#endif // ! MY_SIMUAUDIO_H_
