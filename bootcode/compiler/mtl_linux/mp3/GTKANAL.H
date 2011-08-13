/* 
layerIII enc->dec delay:  1056    (observed)
layerII  enc->dec dealy:   480    (observed)


polyphase 256-16             (dec or enc)
mdct      256+32  (9*32)     (dec or enc) 
total:    512+16


SYNC_FLUSH calculation:
layerIII enc delay:                     528
extra bits to display full fft window:  224
total:                                  752

layerII sync_flush:  should be 464
layerIII sync_flush: should be 752


Also we would like to remove mpg123 decoder delay with 
DECDELAY of -528 (not possible)
so delay by a enough to make the total delay 1 whole frame (1152-528)
*/



#define ENCDELAY 528 
#define DECDELAY (1152-528)

/* use this delay if you want to also include the data from the FFT window */
#define WINDELAY (224 + ENCDELAY)


#ifdef HAVEGTK

/* mpg123 ouput is delayed by 1 frame
 * HOWEVER, sometimes mpg123 doesn't produce a frame until we send the
 * next frame.  Everytime this happens, bump up MPGLAG.  
 */
#define READ_AHEAD 10             /* number of frames to read ahead */
#define MAXMPGLAG READ_AHEAD      /* if the mpg123 lag becomes bigger than this 				     we have to stop */
#define NUMBACK 6                 /* number of frames we can back up */
#define NUMPINFO (NUMBACK+READ_AHEAD+1)

#define BLKSIZE         1024
#define HBLKSIZE        513
#define SBMAX_l 21
#define SBMAX_s 12



typedef struct {
  int frameNum;           /* current frame number */
  int num_samples;        /* number of pcm samples read for this frame */
  double frametime;       /* starting time of frame, in seconds */
  double pcmdata[2][1152+WINDELAY];
  double pcmdata2[2][1152+DECDELAY];
  double energy[2][4][BLKSIZE];
  double xr[2][2][576];
  double mpg123xr[2][2][576];
  double pe[2][4];
  double ms_ratio[2];

  double thr[2][4][SBMAX_l];
  double en[2][4][SBMAX_l];
  double thr_s[2][4][3*SBMAX_s];
  double en_s[2][4][3*SBMAX_s];
  double ers[2][4];

  double xfsf[2][2][SBMAX_l];
  double xfsf_s[2][2][3*SBMAX_s];

  int mpglag;
  int qss[2][2];
  int sub_gain[2][2][3];
  int over[2][2];
  double noise[2][2];
  int blocktype[2][2];
  int scalefac_scale[2][2];
  int mpg123blocktype[2][2];
  int mixed[2][2];
  int mainbits[2][2];
  int LAMEmainbits[2][2];
  int stereo,js,ms_stereo,i_stereo,emph,bitrate,sampfreq,maindata;
  int scfsi,mean_bits,resvsize;
  int totbits;
} plotting_data;
 

int gtkcontrol();

extern int gtkflag;
//extern plotting_data *pinfo;
int decode1file(int fd,short pcm[][1152],int *stereo);
int decode1(char *buf,int len,short pcm[][1152],int stereo);

#endif
