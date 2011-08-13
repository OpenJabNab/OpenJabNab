//-------------------
// MV
// version WIN32
// Sylvain Huet
// Premiere version : 07/01/2003
// Derniere mise a jour : 07/01/2003
//

#ifndef _SIMUNET_
#define _SIMUNET_

#define UDPEVENT (WM_USER+1)
#define TCPEVENT (WM_USER+2)

int simunetinit();

int tcpservercreate(int port);
int tcpopen(char* dstip,int dstport);
int tcpclose(int i);
int tcpsend(int i,char* msg, int len);
int tcpevent(int eventId, int wParam, int lParam);
void tcpenable(int i,int enable);

int udpcreate(int port);
int udpclose(int port);
int udpsend(int localport,char* dstip,int dstport,char* msg, int len);
int udpevent(int eventId, int wParam, int lParam);

#endif

