// VLISP Virtual Machine - 2006 - by Sylvain Huet
// Lowcost IS Powerfull

#include"vmem.h"

#include <string.h>

#ifdef VSIMU
#include<stdio.h>
#include<time.h>
#endif

#include"vloader.h"
#include"vinterp.h"
#include"vaudio.h"

#ifdef VSIMU
#include"linux_simuaudio.h"
#endif
#ifdef VREAL
#include"audio.h"
#endif

char audioFifoPlay[AUDIO_FIFOPLAY];

int play_w;
int play_r;

void audioInit()
{
	play_w=play_r=0;
}

void audioPlayStart(int freq,int bps,int stereo,int trytofeed)
{
	play_w=play_r=0;
#ifdef VSIMU
	PlayStart(freq,stereo,44100,bps,3);
#endif
#ifdef VREAL
	//      set_vlsi_volume(get_adc_value()/2);          //volume on 8bits, 0x00 => maximum
	play_start(trytofeed);
#endif
}

int audioPlayFeed(char *src,int len)
{
	//#ifdef VSIMU
	int i_end;
	if (!src)
	{
#ifdef VSIMU
		PlayEof();
#endif
#ifdef VREAL
        play_eof();
#endif
		return 0;
	}
	i_end=play_r-1;
	if (play_w>=play_r) i_end=AUDIO_FIFOPLAY+play_r-1;
	if (play_w+len>=i_end) len=i_end-play_w;
	else i_end=play_w+len;
	
	if (i_end<=play_w) return 0;
	if (i_end<AUDIO_FIFOPLAY)
	{
		len=i_end-play_w;
		memcpy(audioFifoPlay+play_w,src,len);
		play_w=i_end;
		return len;
	}
	len=AUDIO_FIFOPLAY-play_w;
	memcpy(audioFifoPlay+play_w,src,len);
	src+=len;
	play_w=0;
	i_end-=AUDIO_FIFOPLAY;
	if (i_end) memcpy(audioFifoPlay,src,i_end);
	play_w=i_end;
	return len+i_end;
	//#endif
	/*
	#ifdef VREAL
	set_vlsi_volume(get_adc_value()/2);          //volume on 8bits, 0x00 => maximum
	//Play file
	play_audio((UBYTE*)src,len);
	#endif
	*/
}

void audioPlayStop()
{
#ifdef VSIMU
	PlayStop();
#endif
#ifdef VREAL
	play_stop();
#endif
}

int audioPlayTryFeed(int ask)
{
  int lastw;
  int play_w0=play_w;
  int dispo=play_w-play_r;
  if (dispo<0) dispo+=AUDIO_FIFOPLAY;
  if (dispo>=ask) return 0;
  do
  {
    lastw=play_w;
    dispo=play_w-play_r;
    if (dispo<0) dispo+=AUDIO_FIFOPLAY;

    VPUSH(INTTOVAL(AUDIO_FIFOPLAY-dispo));
    VPUSH(VCALLSTACKGET(sys_start,SYS_CBPLAY));
    if (VSTACKGET(0)==NIL)
    {
      VPULL();
      VPULL();
      return 0;
    }
    interpGo();
    VPULL();
  }
  while(play_w!=lastw);
  if (play_w!=play_w0) return 1;
  return 0;
}


int audioPlayFetchByte()
{
	int res;
	if (play_w==play_r) return -1;
	res=audioFifoPlay[play_r++]&255;
	if (play_r>=AUDIO_FIFOPLAY) play_r-=AUDIO_FIFOPLAY;
	return res;
}

int audioPlayFetch(char* dst,int ask)
{
	int n=0;
	int c;
	audioPlayTryFeed(ask);
	while(n<ask)
	{
		c=audioPlayFetchByte();
		if (c<0) return n;
		dst[n++]=c;
	}
	return n;
}

void audioVol(int vol)
{
#ifdef VSIMU
//	printf("xxxx audioVol %d\n",vol);

	audioSetVolume(255-vol);
#endif
#ifdef VREAL
	set_vlsi_volume((vol&255)/2);
#endif
}

int audioPlayTime()
{
#ifdef VSIMU
	printf("xxxx audioPlayTime\n");
	return 0;
#endif
#ifdef VREAL
	return check_decode_time();
#endif
}

int audioRecStart(int freq,int gain)
{
#ifdef VSIMU
	RecStart(freq,505,4);
#endif
#ifdef VREAL
	rec_start(freq,gain);
#endif
	return 0;
}

int audioRecStop()
{
#ifdef VSIMU
	RecStop();
#endif
#ifdef VREAL
	rec_stop();
#endif
	return 0;
}

char* audioRecFeed_begin(int size)
{
	VPUSH(PNTTOVAL(VMALLOCBIN(size)));
	return (char*)VSTARTBIN(VALTOPNT(VSTACKGET(0)));
}

void audioRecFeed_end()
{
	VPUSH(VCALLSTACKGET(sys_start,SYS_CBREC));
	if (VSTACKGET(0)!=NIL) interpGo();
	else { VPULL();}
	VPULL();
}


#define uint16_t unsigned short
#define int16_t short
#define uint8_t unsigned char
#define int8_t char

static const uint16_t IMA_ADPCMStepTable[89] =
{
	7,	  8,	9,	 10,   11,	 12,   13,	 14,
		16,	 17,   19,	 21,   23,	 25,   28,	 31,
		34,	 37,   41,	 45,   50,	 55,   60,	 66,
		73,	 80,   88,	 97,  107,	118,  130,	143,
		157,	173,  190,	209,  230,	253,  279,	307,
		337,	371,  408,	449,  494,	544,  598,	658,
		724,	796,  876,	963, 1060, 1166, 1282, 1411,
		1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
		3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484,
		7132, 7845, 8630, 9493,10442,11487,12635,13899,
		15289,16818,18500,20350,22385,24623,27086,29794,
		32767
};


static const int IMA_ADPCMIndexTable[8] =
{
	-1, -1, -1, -1, 2, 4, 6, 8,
};


int16_t PredictedValue;
uint8_t StepIndex;
int16_t vmin,vmax;
int vol;

void mydecodeinit(int s0,int st)
{
	PredictedValue=s0;
	StepIndex=st;
	vmin=0;
	vmax=0;
	vol=0;
}

int mydecode(unsigned int adpcm)
{
	int diff;
	int predicedValue;
	int stepIndex = StepIndex;
	int step = IMA_ADPCMStepTable[stepIndex];
	
	stepIndex += IMA_ADPCMIndexTable[adpcm&7];
	if(stepIndex<0)
		stepIndex = 0;
	else if(stepIndex>88)
		stepIndex = 88;
	StepIndex = stepIndex;
	
	diff = step>>3;
	if(adpcm&4)
		diff += step;
	if(adpcm&2)
		diff += step>>1;
	if(adpcm&1)
		diff += step>>2;
	
	predicedValue = PredictedValue;
	if(adpcm&8)
		predicedValue -= diff;
	else
		predicedValue += diff;
	if(predicedValue<-0x8000)
		predicedValue = -0x8000;
	else if(predicedValue>0x7fff)
		predicedValue = 0x7fff;
	PredictedValue = predicedValue;
	//	if (predicedValue>vmax) vmax=predicedValue;
	//	if (predicedValue<vmin) vmin=predicedValue;
	vol+=(predicedValue*predicedValue)>>16;
	return predicedValue;
}

int myvolume()
{
	//	if (vmax>-vmin) return vmax;
	//	return -vmin;
	return vol>>8;
}

int audioRecVol(unsigned char* src,int len,int start)
{
	int s0,st,i;
	
	if (start+256>len) return 0;
	src+=start;
	s0=(src[0]&255)+((src[1]&255)<<8); src+=2;
	st=(src[0]&255)+((src[1]&255)<<8); src+=2;
	mydecodeinit(s0,st);
	for(i=0;i<252;i++)
	{
		int c=(*(src++))&255;
		mydecode(c&15);
		mydecode((c>>4)&15);
	}
	return myvolume();
}

void adpcmdecode(unsigned char* src,char *dstc)
{
	short* dst=(short*)dstc;
	int s0,st,i;
	s0=(src[0]&255)+((src[1]&255)<<8); src+=2;
	st=(src[0]&255)+((src[1]&255)<<8); src+=2;
	mydecodeinit(s0,st);
	*(dst++)=s0;
	for(i=0;i<252;i++)
	{
		int c=(*(src++))&255;
		*(dst++)=mydecode(c&15);
		*(dst++)=mydecode((c>>4)&15);
	}
	
}

int myencode(int16_t pcm16)
{
	int step,diff;
	int predicedValue = PredictedValue;
	int stepIndex = StepIndex;
	
	int delta = pcm16-predicedValue;
	unsigned int value;
	if(delta>=0)
		value = 0;
	else
	{
		value = 8;
		delta = -delta;
	}
	
	step = IMA_ADPCMStepTable[stepIndex];
	diff = step>>3;
	if(delta>step)
	{
		value |= 4;
		delta -= step;
		diff += step;
	}
	step >>= 1;
	if(delta>step)
	{
		value |= 2;
		delta -= step;
		diff += step;
	}
	step >>= 1;
	if(delta>step)
	{
		value |= 1;
		diff += step;
	}
	
	if(value&8)
		predicedValue -= diff;
	else
		predicedValue += diff;
	if(predicedValue<-0x8000)
		predicedValue = -0x8000;
	else if(predicedValue>0x7fff)
		predicedValue = 0x7fff;
	PredictedValue = predicedValue;
	
	stepIndex += IMA_ADPCMIndexTable[value&7];
	if(stepIndex<0)
		stepIndex = 0;
	else if(stepIndex>88)
		stepIndex = 88;
	StepIndex = stepIndex;
	
	return value&15;
}

void adpcmencode(short* src,char *dst)
{
	int delta,stepIndex,i;
	short sample1,sample2;
	short* p;
	
	sample1=src[0];
	sample2=src[1];
	
	PredictedValue = sample1;
	delta = sample2-sample1;
	if(delta<0)
		delta = - delta;
	if(delta>32767)
		delta = 32767;
	stepIndex = 0;
	while(IMA_ADPCMStepTable[stepIndex]<(unsigned)delta)
		stepIndex++;
	StepIndex = stepIndex;
	
	p=(short*)dst;
	p[0]=sample1;
	p[1]=StepIndex;
	
	dst+=4;
	for(i=1;i<505;i+=2)
	{
		*(dst++)=myencode(src[i])+(myencode(src[i+1])<<4);
	}
}

const short alaw[256]={
-5504,-5248,-6016,-5760,-4480,-4224,-4992,-4736,-7552,-7296,-8064,-7808,-6528,-6272,-7040,-6784,-2752,-2624,-3008,-2880,-2240,-2112,-2496,-2368,-3776,-3648,-4032,-3904,-3264,-3136,-3520,-3392,
-22016,-20992,-24064,-23040,-17920,-16896,-19968,-18944,-30208,-29184,-32256,-31232,-26112,-25088,-28160,-27136,-11008,-10496,-12032,-11520,-8960,-8448,-9984,-9472,-15104,-14592,-16128,-15616,-13056,-12544,-14080,-13568,
-344,-328,-376,-360,-280,-264,-312,-296,-472,-456,-504,-488,-408,-392,-440,-424,-88,-72,-120,-104,-24,-8,-56,-40,-216,-200,-248,-232,-152,-136,-184,-168,
-1376,-1312,-1504,-1440,-1120,-1056,-1248,-1184,-1888,-1824,-2016,-1952,-1632,-1568,-1760,-1696,-688,-656,-752,-720,-560,-528,-624,-592,-944,-912,-1008,-976,-816,-784,-880,-848,
5504,5248,6016,5760,4480,4224,4992,4736,7552,7296,8064,7808,6528,6272,7040,6784,2752,2624,3008,2880,2240,2112,2496,2368,3776,3648,4032,3904,3264,3136,3520,3392,
22016,20992,24064,23040,17920,16896,19968,18944,30208,29184,32256,31232,26112,25088,28160,27136,11008,10496,12032,11520,8960,8448,9984,9472,15104,14592,16128,15616,13056,12544,14080,13568,
344,328,376,360,280,264,312,296,472,456,504,488,408,392,440,424,88,72,120,104,24,8,56,40,216,200,248,232,152,136,184,168,
1376,1312,1504,1440,1120,1056,1248,1184,1888,1824,2016,1952,1632,1568,1760,1696,688,656,752,720,560,528,624,592,944,912,1008,976,816,784,880,848};
const short mulaw[256]={
-32124,-31100,-30076,-29052,-28028,-27004,-25980,-24956,-23932,-22908,-21884,-20860,-19836,-18812,-17788,-16764,-15996,-15484,-14972,-14460,-13948,-13436,-12924,-12412,-11900,-11388,-10876,-10364,-9852,-9340,-8828,-8316,
-7932,-7676,-7420,-7164,-6908,-6652,-6396,-6140,-5884,-5628,-5372,-5116,-4860,-4604,-4348,-4092,-3900,-3772,-3644,-3516,-3388,-3260,-3132,-3004,-2876,-2748,-2620,-2492,-2364,-2236,-2108,-1980,
-1884,-1820,-1756,-1692,-1628,-1564,-1500,-1436,-1372,-1308,-1244,-1180,-1116,-1052,-988,-924,-876,-844,-812,-780,-748,-716,-684,-652,-620,-588,-556,-524,-492,-460,-428,-396,
-372,-356,-340,-324,-308,-292,-276,-260,-244,-228,-212,-196,-180,-164,-148,-132,-120,-112,-104,-96,-88,-80,-72,-64,-56,-48,-40,-32,-24,-16,-8,0,
32124,31100,30076,29052,28028,27004,25980,24956,23932,22908,21884,20860,19836,18812,17788,16764,15996,15484,14972,14460,13948,13436,12924,12412,11900,11388,10876,10364,9852,9340,8828,8316,
7932,7676,7420,7164,6908,6652,6396,6140,5884,5628,5372,5116,4860,4604,4348,4092,3900,3772,3644,3516,3388,3260,3132,3004,2876,2748,2620,2492,2364,2236,2108,1980,
1884,1820,1756,1692,1628,1564,1500,1436,1372,1308,1244,1180,1116,1052,988,924,876,844,812,780,748,716,684,652,620,588,556,524,492,460,428,396,
372,356,340,324,308,292,276,260,244,228,212,196,180,164,148,132,120,112,104,96,88,80,72,64,56,48,40,32,24,16,8,0};
const char exposant[256]={
0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};


char alawencode(int pcm)
{
	int exponent,mantissa,sign;
	char alaw;

    sign = (pcm & 0x8000) >> 8;
    if (sign != 0) pcm = -pcm;
    //The magnitude must fit in 15 bits to avoid overflow
    if (pcm > 32635) pcm = 32635;

	exponent=exposant[pcm>>8];

//    exponent = 7;
//    for (expMask = 0x4000; (pcm & expMask) == 0 && exponent>0; exponent--, expMask >>= 1) { }

    mantissa = (pcm >> ((exponent == 0) ? 4 : (exponent + 3))) & 0x0f;

    //The a-law byte bit arrangement is SEEEMMMM
    //(Sign, Exponent, and Mantissa.)
    alaw = (char)(sign | exponent << 4 | mantissa);

    //Last is to flip every other bit, and the sign bit (0xD5 = 1101 0101)
    return (char)(alaw^0xD5);
}

char mulawencode(int pcm) //16-bit
{
	int exponent,mantissa,sign;
	char mulaw;

    //Get the sign bit. Shift it for later
    //use without further modification
    sign = (pcm & 0x8000) >> 8;
    //If the number is negative, make it
    //positive (now it's a magnitude)
    if (sign != 0) pcm = -pcm;
    //The magnitude must be less than 32635 to avoid overflow
    if (pcm > 32635) pcm = 32635;
    //Add 132 to guarantee a 1 in
    //the eight bits after the sign bit
    pcm += 0x84;

    /* Finding the "exponent"
    * Bits:
    * 1 2 3 4 5 6 7 8 9 A B C D E F G
    * S 7 6 5 4 3 2 1 0 . . . . . . .
    * We want to find where the first 1 after the sign bit is.
    * We take the corresponding value from
    * the second row as the exponent value.
    * (i.e. if first 1 at position 7 -> exponent = 2) */
//    exponent = 7;
    //Move to the right and decrement exponent until we hit the 1
	exponent=exposant[pcm>>8];
//    for (expMask = 0x4000; (pcm & expMask) == 0; exponent--, expMask >>= 1) { }

    /* The last part - the "mantissa"
    * We need to take the four bits after the 1 we just found.
    * To get it, we shift 0x0f :
    * 1 2 3 4 5 6 7 8 9 A B C D E F G
    * S 0 0 0 0 0 1 . . . . . . . . . (meaning exponent is 2)
    * . . . . . . . . . . . . 1 1 1 1
    * We shift it 5 times for an exponent of two, meaning
    * we will shift our four bits (exponent + 3) bits.
    * For convenience, we will actually just shift
    * the number, then and with 0x0f. */
    mantissa = (pcm >> (exponent + 3)) & 0x0f;

    //The mu-law byte bit arrangement
    //is SEEEMMMM (Sign, Exponent, and Mantissa.)
    mulaw = (char)(sign | exponent << 4 | mantissa);

    //Last is to flip the bits
    return (char)~mulaw;
}

short alawdecode(unsigned char v)
{
	return alaw[v];
}

short mulawdecode(unsigned char v)
{
	return mulaw[v];
}


void AudioAdp2wav(char* dst,int idst,int ldst,char *src,int isrc,int lsrc,int len)
{
	if ((idst<0)||(isrc<0)||(idst>=ldst)||(isrc>=lsrc)) return;
	if (len&255) return;	// que des blocs de 256
	if ((len>lsrc-isrc)||(((len>>8)*505*2)>ldst-idst)) return;
//	printf("a2w%d\n",len);
	src+=isrc;
	dst+=idst;
	while(len>0)
	{
		adpcmdecode((unsigned char*)src,dst);
		src+=256;
		dst+=505*2;
		len-=256;
	}
}

void AudioWav2adp(char* dst,int idst,int ldst,char *src,int isrc,int lsrc,int len)
{
	int n=0;
	int l=len;

	if ((idst<0)||(isrc<0)||(idst>=ldst)||(isrc>=lsrc)) return;
	while(l>0)
	{
		l-=505*2;
		n++;
	}
	if (l) return;// que des blocs de 505 échantillons 16 bits

	if ((len>lsrc-isrc)||((n<<8)>ldst-idst)) return;
//	printf("w2a%d\n",len);
	src+=isrc;
	dst+=idst;
	while(len>0)
	{
		adpcmencode((short*)src,dst);
		src+=505*2;
		dst+=256;
		len-=505*2;
	}
}

void AudioWav2alaw(char* dst,int idst,int ldst,char *src,int isrc,int lsrc,int len,int mu)
{
	short* p;

	if ((idst<0)||(isrc<0)||(idst>=ldst)||(isrc>=lsrc)) return;
	if (len&1) return;

	if ((len>lsrc-isrc)||((len>>1)>ldst-idst)) return;
//	printf("w2l%d\n",len);
	src+=isrc;
	dst+=idst;
	p=(short*)src;
	len>>=1;
	if (mu)
	{
		while(len--) *(dst++)=mulawencode(*(p++));
	}
	else
	{
		while(len--) *(dst++)=alawencode(*(p++));
	}
}

void AudioAlaw2wav(char* dst,int idst,int ldst,char *src,int isrc,int lsrc,int len,int mu)
{
	short* p;
	unsigned char* q;

	if ((idst<0)||(isrc<0)||(idst>=ldst)||(isrc>=lsrc)) return;

	if ((len>lsrc-isrc)||(len+len>ldst-idst)) return;
//	printf("l2w%d\n",len);
	src+=isrc;
	dst+=idst;
	p=(short*)dst;
	q=(unsigned char*)src;
	if (mu)
	{
		while(len--) *(p++)=mulaw[*(q++)];
	}
	else
	{
		while(len--) *(p++)=alaw[*(q++)];
	}
}

void audioWrite(int reg,int val)
{
#ifdef VSIMU
	printf("xxxx audioWrite %d %d\n",reg,val);
#endif
#ifdef VREAL
	vlsi_write_sci(reg,val);
#endif
}
int audioRead(int reg)
{
#ifdef VSIMU
	printf("xxxx audioRead %d\n",reg);
	return 0;
#endif
#ifdef VREAL
	if (reg==-1) return vlsi_fifo_ready();
	return vlsi_read_sci(reg);
#endif
}
int audioFeed(char *src,int len)
{
#ifdef VSIMU
	printf("xxxx audioFeed %d\n",len);
	return 0;
#endif
#ifdef VREAL
	return vlsi_feed_sdi((uchar*)src,len);
#endif
}
void audioRefresh()
{
#ifdef VSIMU
	printf("xxxx audioRefresh\n");
#endif
#ifdef VREAL
	play_check(0);
	rec_check();
#endif
}
void audioAmpli(int on)
{
#ifdef VSIMU
	printf("xxxx audioAmpli %d\n",on);
#endif
#ifdef VREAL
	vlsi_ampli(on);
#endif
}
