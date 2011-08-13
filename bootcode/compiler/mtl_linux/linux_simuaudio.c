#include <stdio.h>

#include "log.h"

#define A() my_printf(LOG_SIMUAUDIO, "[[%s]]\n", __func__);

int simuaudioinit() { A(); return 0; }

int PlayStart() { A(); return 0; }
int PlayStop() { A(); return 0; }
int PlayEof() { A(); return 0; }

int RecStart(int rate,int ChannelSize,int nbBuffers) { A(); return 0; }
int RecStop() { A(); return 0; }

void audioSetVolume(int vol) { A(); return ; }
