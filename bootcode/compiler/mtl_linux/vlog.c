// VLISP Virtual Machine - 2006 - by Sylvain Huet
// Lowcost IS Powerfull

#include <string.h>

#include"vmem.h"
#include"vloader.h"
#ifdef VSIMU
#include <stdlib.h>
#include<stdio.h>
#include<time.h>
#include <sys/time.h>
#endif
#ifdef VREAL
#include "ML674061.h"
#include "common.h"
#include "irq.h"
#include "spi.h"
#include "led.h"
#include "mem.h"
#include "uart.h"
#include "debug.h"
#include "usbh.h"

#include "delay.h"
#include "audio.h"
#include "motor.h"
#include "usbctrl.h"
#include "ml60842.h"
#include "hcdmem.h"
#include "hcd.h"
#include "inarm.h"
#include "i2c.h"
#include "rt2501usb.h"
#include "mem.h"
#include<stdio.h>
#include<string.h>

#include"led.h"
#include"motor.h"
#include"delay.h"
#include"rfid.h"
#endif
#include"vlog.h"

void logSecho(int p,int nl)
{
	if (p==NIL) consolestr("NIL");
	else consolebin((unsigned char*)VSTARTBIN(VALTOPNT(p)),VSIZEBIN(VALTOPNT(p)));
	if (nl) consolestr(ENDLINE);
}

void logIecho(int i,int nl)
{
	if (i==NIL) consolestr("NIL");
	else consoleint(VALTOINT(i));
	if (nl) consolestr(ENDLINE);
}

extern int currentop;
void logGC()
{
	consolestr("#GC : sp=");consoleint(-vmem_stack);
	consolestr(" hp=");consoleint(vmem_heapindex);
	consolestr(" used=");consoleint((vmem_heapindex-vmem_stack)*100/VMEM_LENGTH);
	consolestr("%"ENDLINE);
        consolestr(" b:");consolehx((int)vmem_heap);
        consolestr(" bc:");consolehx((int)bytecode);
        consolestr(" st:");consolehx(vmem_start);
        consolestr(" op:");consolehx(currentop);
	consolestr(ENDLINE);

}


// pour le firmware, le "fichier" ouvert est toujours l'eeprom

int sysLoad(char *dst,int i,int ldst,char *filename,int j,int len)
{
#ifdef VSIMU
	FILE *f;
	if ((j<0)||(i<0)||(len<=0)) return 0;
	if (i+len>ldst) len=ldst-i;
	if (len<=0) return 0;
	f=fopen(filename,"rb");
	if (!f) return 0;
	fseek(f,j,SEEK_SET);
	len=fread(dst,1,len,f);
	fclose(f);
	return len;
#endif
#ifdef VREAL
/*        set_vlsi_volume(0);          //volume on 8bits, 0x00 => maximum
        encode_adpcm((UBYTE*)dst+i,(ldst-i-256)>>8, j);
        return 0;
*/
	if ((j<0)||(i<0)||(len<=0)) return 0;
	if (i+len>ldst) len=ldst-i;
	if (len<=0) return 0;
	if (j+len>4096) len=4096-j;
	if (len<=0) return 0;
        read_uc_flash(j,(unsigned char*)dst,len);
        return len;
#endif
}

uchar buffer_temp[4096];

// pour le firmware, le "fichier" ouvert est toujours l'eeprom
int sysSave(char *dst,int i,int ldst,char *filename,int j,int len)
{
#ifdef VSIMU
	FILE *f;
	if ((j<0)||(i<0)||(len<=0)) return 0;
	if (i+len>ldst) len=ldst-i;
	if (len<=0) return 0;
	f=fopen(filename,"rb+");
	if (!f) f=fopen(filename,"wb+");
	if (!f) return 0;
	fseek(f,j,SEEK_SET);
	len=fwrite(dst,1,len,f);
	fclose(f);
	return len;
#endif
#ifdef VREAL
	if ((j<0)||(i<0)||(len<=0)) return 0;
	if (i+len>ldst) len=ldst-i;
	if (len<=0) return 0;
	if (j+len>4096) len=4096-j;
	if (len<=0) return 0;
  __disable_interrupt();
        write_uc_flash(j,(unsigned char*)dst,len,buffer_temp);
  __enable_interrupt();
        return len;
#endif
}

static int firstTimeSet = 0;
static struct timeval firstTime;

int sysTimems()
{
#ifdef VSIMU
	int res;
	struct timeval tv;
	if (firstTimeSet) {
		gettimeofday(&tv, NULL);
		res = (tv.tv_sec - firstTime.tv_sec) * 1000;
		res += (tv.tv_usec - firstTime.tv_usec) / 1000;
	} else {
		gettimeofday(&firstTime, NULL);
		firstTimeSet = 1;
		res = 0;
	}
	return res;
#endif // ! VSIMU
#ifdef VREAL
        return counter_timer;
#endif
}

int sysTime()
{
#ifdef VSIMU
	int res;
	static int firstTimeSet = 0;
	static struct timeval firstTime;
	struct timeval tv;
	if (firstTimeSet) {
		gettimeofday(&tv, NULL);
		res = (tv.tv_sec - firstTime.tv_sec);
		res += (tv.tv_usec >= firstTime.tv_usec) ? 0 : -1;
	} else {
		gettimeofday(&firstTime, NULL);
		firstTimeSet = 1;
		res = 0;
	}
	return res;
#endif
#ifdef VREAL
	return counter_timer_s;
#endif
}


int rndval;

// retourne une valeur aléatoire entre 0 et 65535
int sysRand()
{
	rndval=rndval*0x1234567+11;
	return (rndval>>8)&0xffff;
}
void sysSrand(int seed)
{
	rndval=seed;
}


void sysCpy(char *dst,int i,int ldst,char *src,int j,int lsrc,int len)
{
	if ((i<0)||(j<0)||(len<=0)) return;
        if (i+len>ldst) len=ldst-i;
        if (len<=0) return;
	if (j+len>lsrc) len=lsrc-j;
        if (len<=0) return;
	dst+=i;
	src+=j;
	while((len--)>0) *(dst++)=*(src++);
}

int sysCmp(char *dst,int i,int ldst,char *src,int j,int lsrc,int len)
{
	if ((i<0)||(j<0)||(len<=0)) return 0;
	if ((i+len>ldst)&&(j+len>lsrc))
        {
          if (ldst-i>lsrc-j) len=ldst-i;
          else len=lsrc-j;
        }
	dst+=i;
	src+=j;
	while((len--)>0) if (((unsigned char)*dst)>((unsigned char)*src)) return 1;
	else  if (((unsigned char)*(dst++))<((unsigned char)*(src++))) return -1;
	return 0;
}

int mystrcmp(char *dst,char *src,int len)
{
	while((len--)>0) if ((*(dst++))!=(*(src++))) return 1;
	return 0;
}

void mystrcpy(char *dst,char *src,int len)
{
	while((len--)>0) *(dst++)=*(src++);
}

int sysFind(char *dst,int i,int ldst,char *src,int j,int lsrc,int len)
{
	if ((j<0)||(j+len>lsrc)) return NIL;
	src+=j;
	if (i<0) i=0;
	while(i+len<=ldst)
	{
		if (!mystrcmp(dst+i,src,len)) return INTTOVAL(i);
		i++;
	}
	return NIL;
}

int sysFindrev(char *dst,int i,int ldst,char *src,int j,int lsrc,int len)
{
	if ((j<0)||(j+len>lsrc)) return NIL;
	src+=j;
	if(i+len>ldst) i=ldst-len;
	while(i>=0)
	{
		if (!mystrcmp(dst+i,src,len)) return INTTOVAL(i);
		i--;
	}
	return NIL;
}

int sysStrgetword(unsigned char *src,int len,int ind)
{
	int n;
  if ((ind<0)||(ind+2>len)) return -1;
  n=(src[ind]<<8)+src[ind+1];
  return n;
}

void sysStrputword(unsigned char *src,int len,int ind,int val)
{
  if ((ind<0)||(ind+2>len)) return;
  src[ind+1]=val; val>>=8;
  src[ind]=val;
}

// lecture d'une chaîne décimale (s'arrête au premier caractère incorrect)
int sysAtoi(char* src)
{
  int x,c,s;
  x=s=0;
  if ((*src)=='-') { s=1; src++; }
  while(c=*src++)
  {
    if ((c>='0')&&(c<='9')) x=(x*10)+c-'0';
    else return (s?(-x):x);
  }
  return (s?(-x):x);
}

// lecture d'une chaîne hexadécimale (s'arrête au premier caractère incorrect)
int sysHtoi(char* src)
{
	int x,c;
	x=0;
	while(c=*src++)
	{
		if ((c>='0')&&(c<='9')) x=(x<<4)+c-'0';
		else if ((c>='A')&&(c<='F')) x=(x<<4)+c-'A'+10;
		else if ((c>='a')&&(c<='f')) x=(x<<4)+c-'a'+10;
		else return x;
	}
	return x;
}
void sysCtoa(int c)
{
  unsigned char res[1];
  res[0]=c;
  VPUSH(PNTTOVAL(VMALLOCSTR((char*)res,1)));
}

const int itoarsc[10]={
  1000000000,100000000,10000000,
  1000000   ,100000   ,10000,
  1000      ,100      ,10,
  1
};;
void sysItoa(int v)
{
  char res[16];
  int ires=0;
  if (v==0)
  {
    res[ires++]='0';
  }
  else
  {
    int start=1;
    int imul=0;
    if (v<0)
    {
      v=-v;
      res[ires++]='-';
    }
    while(imul<10)
    {
      int k=0;
      while(v>=itoarsc[imul])
      {
        k++;
        v-=itoarsc[imul];
      }
      if ((k)||(!start))
      {
        start=0;
        res[ires++]='0'+k;
      }
      imul++;
    }
  }

  VPUSH(PNTTOVAL(VMALLOCSTR(res,ires)));

}

void sysItoh(int v)
{
  char res[16];
  int ires=0;
  if (v==0)
  {
    res[ires++]='0';
  }
  else
  {
    int start=1;
    int imul=28;
    while(imul>=0)
    {
      int c=(v>>imul)&15;
      if ((c)||(!start))
      {
        start=0;
        res[ires++]=(c<10)?'0'+c:'a'+c-10;
      }
      imul-=4;
    }
  }
  VPUSH(PNTTOVAL(VMALLOCSTR(res,ires)));
}

void sysCtoh(int c)
{
  unsigned char res[2];
  int v=(c>>4)&15;
  res[0]=(v<10)?'0'+v:'a'+v-10;
  v=c&15;
  res[1]=(v<10)?'0'+v:'a'+v-10;
  VPUSH(PNTTOVAL(VMALLOCSTR((char*)res,2)));
}

void sysItobin2(int c)
{
  unsigned char res[2];
  res[1]=c;
  c>>=8;
  res[0]=c;
  VPUSH(PNTTOVAL(VMALLOCSTR((char*)res,2)));
}

int sysListswitch(int p,int key)
{
  while(p!=NIL)
  {
    int q=VALTOPNT(VFETCH(p,0));
    if ((q!=NIL)&&(VFETCH(q,0)==key)) return VFETCH(q,1);
    p=VALTOPNT(VFETCH(p,1));
  }
  return NIL;
}

int sysListswitchstr(int p,char* key)
{
  while(p!=NIL)
  {
    int q=VALTOPNT(VFETCH(p,0));
    if (q!=NIL)
    {
      int r=VALTOPNT(VFETCH(q,0));
      if ((r!=NIL)&&(!strcmp(VSTARTBIN(r),key))) return VFETCH(q,1);
    }
    p=VALTOPNT(VFETCH(p,1));
  }
  return NIL;
}

void simuSetLed(int i,int val);
void set_motor_dir(int num_motor, int sens);
int get_motor_val(int i);
int getButton();
int get_button3();
char* get_rfid();

void sysLed(int led,int col)
{
#ifdef VSIMU
	simuSetLed(led,col);
#endif
#ifdef VREAL
        set_led((UWORD)led,(UWORD)col);
#endif
}

void sysMotorset(int motor,int sens)
{
#ifdef VSIMU
	set_motor_dir(motor,sens);
#endif
#ifdef VREAL
//        char buffer[256];
        motor=1+(motor&1);

//        sprintf(buffer,"setmotor %d sens %d\r\n",motor,sens);
//        consolestr(buffer);

        if (sens==0) stop_motor(motor);
        else run_motor(motor,255,(sens>0)?REVERSE:FORWARD/*:REVERSE*/);
#endif
}

int kmotor[3];
int kvmotor[3];

int sysMotorget(int motor)
{
#ifdef VSIMU
	return get_motor_val(motor);
#endif
#ifdef VREAL
//        char buffer[256];
        int k,kx;
        motor=1+(motor&1);
        kx=(int)get_motor_position(motor);
/*        k=(int)get_motor_position(motor);
        if (kmotor[motor]!=k)
        {
          kmotor[motor]=k;
          kvmotor[motor]++;
        }
        kx=kvmotor[motor];
*/

//        sprintf(buffer,"getmotor %d pos %x / %x\r\n",motor,k,kx);
//        if(motor==2)
//        consolestr(buffer);
        return kx;
#endif
}

int sysButton2()
{
#ifdef VSIMU
	return getButton();
#endif
#ifdef VREAL
        return push_button_value();
#endif
}

int sysButton3()
{
#ifdef VSIMU
	return get_button3();
#endif
#ifdef VREAL
	return 255-get_adc_value();
#endif
}

char* sysRfidget()
{
#ifdef VSIMU
        return get_rfid();
#endif
#ifdef VREAL
        return get_rfid_first_device();
#endif
}

void sysReboot()
{
#ifdef VSIMU
    printf("REBOOT NOW.....");
    getchar();
    exit(0);
#endif
#ifdef VREAL
    reset_uc();
#endif

}

void sysFlash(char* firmware,int len)
{
#ifdef VSIMU
    printf("REBOOT AND FLASH NOW.....");
    getchar();
    exit(0);
#endif
#ifdef VREAL
  __disable_interrupt();
  flash_uc((unsigned char*)firmware,len,buffer_temp);
#endif

}

const uchar inv8[128]=
{
1,171,205,183,57,163,197,239,241,27,61,167,41,19,53,223,225,139,173,151,25,131,165,207,209,251,29,135,9,243,21,191,193,107,141,119,249,99,133,175,177,219,253,103,233,211,245,159,161,75,109,87,217,67,101,143,145,187,221,71,201,179,213,127,129,43,77,55,185,35,69,111,113,155,189,39,169,147,181,95,97,11,45,23,153,3,37,79,81,123,157,7,137,115,149,63,65,235,13,247,121,227,5,47,49,91,125,231,105,83,117,31,33,203,237,215,89,195,229,15,17,59,93,199,73,51,85,255
};

int decode8(uchar* src,int len,uchar key,uchar alpha)
{
	while(len--)
	{
		uchar v=((*src)-alpha)*key;
		*(src++)=v;
		key=v+v+1;
	}
	return key;
}

int encode8(uchar* src,int len,uchar key,uchar alpha)
{
	while(len--)
	{
		uchar v=*src;
		*(src++)=alpha+(v*inv8[key>>1]);
		key=v+v+1;
	}
	return key;
}



int sysCrypt(char* src,int indexsrc,int len,int lensrc,unsigned int key,int alpha)
{
  if ((indexsrc<0)||(indexsrc+len>lensrc)||(len<=0)) return -1;
  return encode8(src+indexsrc,len,key,alpha);
}
int sysUncrypt(char* src,int indexsrc,int len,int lensrc,unsigned int key,int alpha)
{
  if ((indexsrc<0)||(indexsrc+len>lensrc)||(len<=0)) return -1;
  return decode8(src+indexsrc,len,key,alpha);
}


