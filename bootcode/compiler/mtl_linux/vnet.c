// VLISP Virtual Machine - 2006 - by Sylvain Huet
// Lowcost IS Powerfull
#include"vmem.h"
#include"properties.h"

#ifdef VSIMU
#include <string.h>
#include<stdio.h>
#include<time.h>
#include "log.h"
int netstatesimu=1;
#endif
#ifdef VREAL
#include "rt2501usb.h"
#endif
#include"vloader.h"
#include"vnet.h"
#include"vlog.h"
#include"vinterp.h"
int netState()
{
#ifdef VSIMU
        return netstatesimu;
#endif
#ifdef VREAL
      return rt2501_state();
#endif
}

int netSend(char* src,int indexsrc,int lentosend,int lensrc,char* macdst,int inddst,int lendst,int speed)
{
#ifdef VSIMU
        my_printf(LOG_SIMUNET, "xxxx netSend\n");
		return -1;
#endif
#ifdef VREAL
  if (indexsrc<0) return -1;
  if (indexsrc+lentosend>lensrc) lentosend=lensrc-indexsrc;
  if (lentosend<=0) return -1;
  if (inddst<0) return -1;
  if (inddst+6>lendst) return -1;
  return rt2501_send((const unsigned char *)(src+indexsrc),lentosend,(const unsigned char *)(macdst+inddst),speed,1);
#endif
}

int netCb(char* src,int lensrc,char* macsrc)
{
  VPUSH(PNTTOVAL(VMALLOCSTR(src,lensrc)));
  VPUSH(PNTTOVAL(VMALLOCSTR(macsrc,6)));
  VPUSH(VCALLSTACKGET(sys_start,SYS_CBTCP));
  if (VSTACKGET(0)!=NIL) interpGo();
  else { VPULL();VPULL();}
  VPULL();
  return 0;
}

extern unsigned char rt2501_mac[6];

#ifdef VSIMU
int htoi2(char* src)
{
	char buf[3];
	buf[0]=src[0];
	buf[1]=src[1];
	buf[2]=0;
	return sysHtoi(buf);
}
#endif

char mac[6];

char* netMac()
{
#ifdef VSIMU
	int i;
	char *p=PropGet("MAC");
	for(i=0;i<6;i++) mac[i]=htoi2(&p[i*2]);
	return mac;
//	return "abcdef";
#endif
#ifdef VREAL
  return (char*) rt2501_mac;
#endif
}

int netChk(char* src,int indexsrc,int lentosend,int lensrc,unsigned int val)
{
  unsigned short* p;

  if (indexsrc<0) return val;
  if (indexsrc+lentosend>lensrc) lentosend=lensrc-indexsrc;
  if (lentosend<=0) return val;

  src+=indexsrc;
  p=(unsigned short*)src;

  val=((val<<8)&0xff00)+((val>>8)&0xff);
  while(lentosend>1)
  {
	  val+=*(p++);
	  lentosend-=2;
  }
  if (lentosend) val+=*(unsigned char*)p;

  val=(val>>16)+(val&0xffff);
  val=(val>>16)+(val&0xffff);
  val=((val<<8)&0xff00)+((val>>8)&0xff);
  return val;
}

void netSetmode(int mode,char* ssid,int chn)
{
#ifdef VSIMU
        my_printf(LOG_SIMUNET, "xxxx netSetmode %d %s %d\n",mode,ssid,chn);
		netstatesimu=(mode)?5:4;
#endif
#ifdef VREAL
  rt2501_setmode(mode,ssid,chn);
#endif
}


int nscan;
#ifdef VREAL
void netScan_(struct rt2501_scan_result *scan_result, void *userparam)
{
  char buf[256];
  sprintf(buf,">>> %s %d %d %d %d\n",scan_result->ssid,scan_result->rssi,scan_result->channel,scan_result->rateset,scan_result->encryption);
  consolestr((UBYTE*)buf);
  VPUSH(PNTTOVAL(VMALLOCSTR(scan_result->ssid,strlen(scan_result->ssid))));
  VPUSH(PNTTOVAL(VMALLOCSTR((char*)scan_result->mac,6)));
  VPUSH(PNTTOVAL(VMALLOCSTR((char*)scan_result->bssid,6)));
  VPUSH(INTTOVAL(scan_result->rssi));
  VPUSH(INTTOVAL(scan_result->channel));
  VPUSH(INTTOVAL(scan_result->rateset));
  VPUSH(INTTOVAL(scan_result->encryption));
  VMKTAB(7);
  nscan++;
}
#endif

void netScan(char* ssid)
{
  nscan=0;
#ifdef VSIMU
  VPUSH(PNTTOVAL(VMALLOCSTR("foo",3)));
  VPUSH(PNTTOVAL(VMALLOCSTR("xyzxyz",6)));
  VPUSH(PNTTOVAL(VMALLOCSTR("765432",6)));
  VPUSH(INTTOVAL(-20));
  VPUSH(INTTOVAL(11));
  VPUSH(INTTOVAL(1));
  VPUSH(INTTOVAL(0));
  VMKTAB(7);
  nscan++;
  VPUSH(PNTTOVAL(VMALLOCSTR("foo",3)));
  VPUSH(PNTTOVAL(VMALLOCSTR("xyzxyz",6)));
  VPUSH(PNTTOVAL(VMALLOCSTR("765432",6)));
  VPUSH(INTTOVAL(-10));
  VPUSH(INTTOVAL(11));
  VPUSH(INTTOVAL(1));
  VPUSH(INTTOVAL(0));
  VMKTAB(7);
  nscan++;
  VPUSH(PNTTOVAL(VMALLOCSTR("zzzz",3)));
  VPUSH(PNTTOVAL(VMALLOCSTR("xyzxyz",6)));
  VPUSH(PNTTOVAL(VMALLOCSTR("765432",6)));
  VPUSH(INTTOVAL(-10));
  VPUSH(INTTOVAL(11));
  VPUSH(INTTOVAL(1));
  VPUSH(INTTOVAL(0));
  VMKTAB(7);
  nscan++;
  VPUSH(PNTTOVAL(VMALLOCSTR("",3)));
  VPUSH(PNTTOVAL(VMALLOCSTR("xyzxyz",6)));
  VPUSH(PNTTOVAL(VMALLOCSTR("765432",6)));
  VPUSH(INTTOVAL(-10));
  VPUSH(INTTOVAL(11));
  VPUSH(INTTOVAL(1));
  VPUSH(INTTOVAL(0));
  VMKTAB(7);
  nscan++;
  VPUSH(PNTTOVAL(VMALLOCSTR("a\"a<a'a  a\\a",strlen("a\"a<a'a  a\\a"))));
  VPUSH(PNTTOVAL(VMALLOCSTR("xyzxyz",6)));
  VPUSH(PNTTOVAL(VMALLOCSTR("765432",6)));
  VPUSH(INTTOVAL(-10));
  VPUSH(INTTOVAL(11));
  VPUSH(INTTOVAL(1));
  VPUSH(INTTOVAL(3));
  VMKTAB(7);
  nscan++;
  VPUSH(PNTTOVAL(VMALLOCSTR("foo",3)));
  VPUSH(PNTTOVAL(VMALLOCSTR("xyzxyz",6)));
  VPUSH(PNTTOVAL(VMALLOCSTR("765432",6)));
  VPUSH(INTTOVAL(-30));
  VPUSH(INTTOVAL(11));
  VPUSH(INTTOVAL(1));
  VPUSH(INTTOVAL(0));
  VMKTAB(7);
  nscan++;
#endif
#ifdef VREAL
  rt2501_scan(ssid, netScan_, NULL);
#endif
  VPUSH(NIL);
  while(nscan--) VMKTAB(2);
}

void netAuth(char* ssid,char* mac,char* bssid,int chn,int rate,int authmode,int encrypt,char* key)
{
#ifdef VSIMU
        my_printf(LOG_SIMUNET, "xxxx netAuth %s %d %d %d %d\n",ssid,chn,rate,authmode,encrypt);
		netstatesimu=4;
#endif
#ifdef VREAL
  rt2501_auth((const char*)ssid,(const unsigned char*)mac,(const unsigned char*)bssid,chn,rate,authmode,encrypt,(const unsigned char*)key);
#endif
}

void netSeqAdd(unsigned char* seq,int n)
{
  unsigned char res[4];
  unsigned int val;
  val=(seq[0]<<24)+(seq[1]<<16)+(seq[2]<<8)+seq[3];
  val+=n;
  res[3]=val; val>>=8;
  res[2]=val; val>>=8;
  res[1]=val; val>>=8;
  res[0]=val;
  VPUSH(PNTTOVAL(VMALLOCSTR((char*)res,4)));
}

void mypassword_to_pmk(const char *password, char *ssid, int ssidlength, unsigned char *pmk);

void netPmk(char* ssid,char* key,char* buf)
{
#ifdef VSIMU
	my_printf(LOG_SIMUNET, "xxxx netPmk %s %s\n",ssid,key);
	strcpy(buf,"01234567012345670123456701234567");
#endif
#ifdef VREAL
//      strcpy(buf,"01234567012345670123456701234567");
      mypassword_to_pmk(key,ssid,strlen(ssid), (unsigned char*)buf);
#endif
}

int netRssi()
{
#ifdef VSIMU
	return -20;
#endif
#ifdef VREAL
	return rt2501_rssi_average();
#endif
}

