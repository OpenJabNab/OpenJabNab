// VLISP Virtual Machine - 2006 - by Sylvain Huet
// Lowcost IS Powerfull

#ifndef _NET_
#define _NET_

int netState();
int netSend(char* src,int indexsrc,int lentosend,int lensrc,char* macdst,int inddst,int lendst,int speed);
int netCb(char* src,int lensrc,char* macsrc);
char* netMac();
int netChk(char* src,int indexsrc,int lentosend,int lensrc,unsigned int val);
void netSetmode(int mode,char* ssid,int chn);
void netScan(char* ssid);
void netAuth(char* ssid,char* mac,char* bssid,int chn,int rate,int authmode,int encrypt,char* key);
void netSeqAdd(unsigned char* seq,int n);
void netPmk(char* ssid,char* key,char* buf);
int netRssi();
#endif

