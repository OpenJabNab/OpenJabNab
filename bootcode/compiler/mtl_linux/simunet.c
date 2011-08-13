// Librairie Sound

#include<windows.h>
#include<winsock.h>
#include<stdio.h>
#include"simunet.h"
#include"vmem.h"
#include"vloader.h"
#include"vinterp.h"

extern HWND mainwin;

#define TCPMAX 128
#define UDPMAX 128

#define SOCKETWOULDBLOCK (WSAGetLastError()==WSAEWOULDBLOCK)

SOCKET tcp_sock[TCPMAX];	// =INVALID_SOCKET -> disponible
int tcp_enable[TCPMAX];
SOCKET udp_sock[UDPMAX];
int udp_port[UDPMAX]; // port=0 -> disponible

int inet_addr_bin(char *ip)
{
	return *(int*)ip;
}

int simunetinit()
{
	int i;
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD( 1, 1 );
	if (WSAStartup( wVersionRequested, &wsaData )) return -1;
	for(i=0;i<TCPMAX;i++) tcp_sock[i]=INVALID_SOCKET;
	for(i=0;i<UDPMAX;i++) udp_port[i]=0;
	return 0;
}

int tcpgetfree()
{
	int i;
	for(i=0;i<TCPMAX;i++) if (tcp_sock[i]==INVALID_SOCKET)
	{
		tcp_enable[i]=1;
		return i;
	}
	return -1;
}
int tcpbysock(SOCKET s)
{
	int i;
	for(i=0;i<TCPMAX;i++) if (tcp_sock[i]==s) return i;
	return -1;
}
int tcpservercreate(int port)
{
	int opt=1;
	SOCKADDR_IN ina;
	SOCKET socksrv;
	int i=tcpgetfree();
	if (i<0) return i;

	socksrv=socket(PF_INET,SOCK_STREAM,0);
	if (socksrv==INVALID_SOCKET)
	{
		return -1;
	}
	setsockopt (socksrv, SOL_SOCKET, SO_REUSEADDR,(char*) &opt, sizeof (opt));
	if (WSAAsyncSelect(socksrv,mainwin,TCPEVENT,(FD_ACCEPT))==SOCKET_ERROR)
	{
		closesocket(socksrv);
		return -1;
	}
	ina.sin_family = PF_INET;
	ina.sin_port   = htons((unsigned short)port);
	ina.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(socksrv,(struct sockaddr*)&ina,sizeof(ina))!=0)
	{
		printf("Sockets : Tcp port %d busy\n",port);
		closesocket(socksrv);
		return -1;
	}
	if (listen(socksrv,3)!=0)
	{
		printf("Sockets : Tcp port %d listen error\n",port);
		closesocket(socksrv);
		return -1;
	}
	printf("Sockets : create Tcp server :%d (socket=%d)\n",port,socksrv);
	tcp_sock[i]=socksrv;
	
	return 0;
}


int tcpopen(char* dstip,int dstport)
{
	SOCKET socktcp;
	SOCKADDR_IN ina;
	int opt=1;

	int i=tcpgetfree();
	if (i<0) return i;

	socktcp=socket(PF_INET,SOCK_STREAM,0);
	if (socktcp==INVALID_SOCKET) return -1;

	if (WSAAsyncSelect(socktcp,mainwin,TCPEVENT,(FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE))==SOCKET_ERROR)
	{
		closesocket(socktcp);
		return -1;
	}
	ina.sin_family = PF_INET;
	ina.sin_port   = htons((unsigned short)dstport);
	ina.sin_addr.s_addr = inet_addr_bin(dstip);

	if ( (connect(socktcp,(struct sockaddr *)&ina,sizeof(ina))!=0)
		&&(!SOCKETWOULDBLOCK))
	{
		printf("Sockets : Tcp connect failed\n");
		closesocket(socktcp);
		return -1;
	}
	printf("Sockets : create Tcp %s:%d (socket=%d)\n",dstip,dstport,socktcp);
	
	tcp_sock[i]=socktcp;
	return i;
}
int tcpclose(int i)
{
	if ((i>=0)&&(i<TCPMAX)&&(tcp_sock[i]!=INVALID_SOCKET))
	{
		printf("Sockets : Tcp close %d\n",tcp_sock[i]);
		closesocket(tcp_sock[i]);
		tcp_sock[i]=INVALID_SOCKET;
	}
	return i;
}

void tcpenable(int i,int enable)
{
	if ((i>=0)&&(i<TCPMAX)&&(tcp_sock[i]!=INVALID_SOCKET))
	{
		char buf[16];
		if (enable!=tcp_enable[i])
		{
			tcp_enable[i]=enable;
			if (enable)	recv(tcp_sock[i],buf,0,0);
		}
	}
}

int tcpsend(int i,char* msg, int len)
{
	if ((i>=0)&&(i<TCPMAX)&&(tcp_sock[i]!=INVALID_SOCKET))
	{
		int res=send(tcp_sock[i],msg,len,0);
		if (res<0)
		{
			if (SOCKETWOULDBLOCK) res=0;
			else res=-1;
		}
		return res;
	}
	return -1;
}
int tcpevent(int eventId, int wParam, int lParam)
{
	int NetEvent,NetError;
	SOCKET sock=(SOCKET)wParam;
	int i=tcpbysock(sock);
//	printf("tcpevent\n");
	if (i<0) return 0;
	NetEvent=WSAGETSELECTEVENT(lParam);
	NetError=WSAGETSELECTERROR(lParam);
	if (NetError)
	{
		printf("Sockets : NetError %d on %d\n",NetError,i);
		VPUSH(INTTOVAL(i));
		VPUSH(INTTOVAL(-1));
		VPUSH(NIL);
		VPUSH(VCALLSTACKGET(sys_start,SYS_CBTCP));
		interpGo();
		VPULL();
		return 1;
	}
	if (NetEvent==FD_ACCEPT)
	{
		SOCKADDR_IN cor;
		SOCKET ns;
		int sizecor;
		int ni,ip,port;
		char buf[16];
		
		sizecor=sizeof(cor);
		ns=accept(sock,(struct sockaddr*)&cor,&sizecor);
		if (ns==INVALID_SOCKET) return 1;

		ni=tcpgetfree();
		if (ni<0)
		{
			closesocket(ns);
			return 1;
		}

		ip=cor.sin_addr.s_addr;
		port=ntohs(cor.sin_port);
		
		if (WSAAsyncSelect(ns,mainwin,TCPEVENT,
			(FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE))==SOCKET_ERROR)
		{
			closesocket(ns);
			return 1;
		}
		tcp_sock[ni]=ns;

		printf("Sockets : accept Tcp from %x:%d (socket=%d)\n",ip,port,ns);
		VPUSH(INTTOVAL(ni));
		VPUSH(INTTOVAL(2));
		sprintf(buf,"%d",i);
		VPUSH(PNTTOVAL(VMALLOCSTR(buf,strlen(buf))));
		VPUSH(VCALLSTACKGET(sys_start,SYS_CBTCP));
		if (VSTACKGET(0)!=NIL) interpGo();
		else { VPULL();VPULL();VPULL();}
		VPULL();
		return 1;
	}

	if (NetEvent==FD_WRITE)
	{
		printf("Sockets : Write event on %d\n",i);
		VPUSH(INTTOVAL(i));
		VPUSH(INTTOVAL(0));
		VPUSH(NIL);
		VPUSH(VCALLSTACKGET(sys_start,SYS_CBTCP));
		if (VSTACKGET(0)!=NIL) interpGo();
		else { VPULL();VPULL();VPULL();}
		VPULL();
		return 1;
	}
	if ((NetEvent==FD_READ)||(NetEvent==FD_CLOSE))
	{
		char buf[1024];
		int res;
		if (!tcp_enable[i])
		{
//			Sleep(10);
//			recv(sock,buf,0,0);
//			printf("disabled\n");
			return 1;
		}

		printf("Sockets : Read event on %d\n",sock);
		res=recv(sock,buf,1024,0);

		VPUSH(INTTOVAL(i));
		if (res>0)
		{
			VPUSH(INTTOVAL(1));
			VPUSH(PNTTOVAL(VMALLOCSTR(buf,res)));
		}
		else
		{
			VPUSH(INTTOVAL(-1));
			VPUSH(NIL);
		}
		VPUSH(VCALLSTACKGET(sys_start,SYS_CBTCP));
		if (VSTACKGET(0)!=NIL) interpGo();
		else { VPULL();VPULL();VPULL();}
		VPULL();
		return 1;
	}
	return 0;
}

int udpbyport(int p)
{
	int i;
	for(i=0;i<UDPMAX;i++) if (udp_port[i]==p) return i;
	return -1;
}
int udpgetfree()
{
	return udpbyport(0);
}
int udpbysock(SOCKET s)
{
	int i;
	for(i=0;i<UDPMAX;i++) if ((udp_port[i])&&(udp_sock[i]==s)) return i;
	return -1;
}

int udpcreate(int port)
{
	SOCKET sockudp;
	SOCKADDR_IN ina;
	int opt=1;
	int i;

	udpclose(port);
	
	i=udpgetfree();
	if (i<0) return i;

	sockudp=socket(PF_INET,SOCK_DGRAM,0);
	if (sockudp==INVALID_SOCKET) return -1;

	if (WSAAsyncSelect(sockudp,mainwin,UDPEVENT,(FD_READ))==SOCKET_ERROR)
	{
		closesocket(sockudp);
		return -1;
	}
	ina.sin_family = PF_INET;
	ina.sin_port   = htons((unsigned short)port);
	ina.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockudp,(struct sockaddr*)&ina,sizeof(ina))!=0)
	{
		printf("Sockets : Udp port %d busy\n",port);
		closesocket(sockudp);
		return -1;
	}
	setsockopt(sockudp, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof (opt));
	setsockopt(sockudp, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof (opt));
	printf("Sockets : create Udp :%d (socket=%d)\n",port,sockudp);
	
	udp_port[i]=port;
	udp_sock[i]=sockudp;
	return i;
}

int udpclose(int port)
{
	int i=udpbyport(port);
	if (i>=0)
	{
		closesocket(udp_sock[i]);
		udp_port[i]=0;
	}
	return i;
}

int udpsend(int localport,char* dstip,int dstport,char* msg, int len)
{
	SOCKADDR_IN ina;
	int i;

	i=udpbyport(localport);
	if (i<0) i=udpcreate(localport);
	if (i>=0)
	{
		ina.sin_family = PF_INET;
		ina.sin_port   = htons((unsigned short)dstport);
		ina.sin_addr.s_addr = inet_addr_bin(dstip);
		printf("send %d bytes on chn %d\n",len,i);
		sendto(udp_sock[i],msg,len,0,(struct sockaddr *)&ina,sizeof(ina));
		return 0;
	}
	return -1;
}

int udpevent(int eventId, int wParam, int lParam)
{
	int NetEvent,NetError;
	SOCKET sock=(SOCKET)wParam;
	int i=udpbysock(sock);
	printf("Sockets : UDP event\n");

	if (i<0) return 0;

	NetEvent=WSAGETSELECTEVENT(lParam);
	NetError=WSAGETSELECTERROR(lParam);
	if (NetError)
	{
		return 1;
	}
	if (NetEvent==FD_READ)
	{
		char buf[4096];
		char *ip;
		SOCKADDR_IN add;
		int l=sizeof(add);
		int res=recvfrom(sock,buf,4096,0,(struct sockaddr *)&add,&l);
		if (res<0) return 1;
		
		ip=inet_ntoa(add.sin_addr);
		printf("Sockets : UDP Read %d bytes on :%d from %s:%d\n",res,udp_port[i],ip,ntohs(add.sin_port));
		VPUSH(INTTOVAL(i));
		VPUSH(PNTTOVAL(VMALLOCSTR(buf,res)));
		VPUSH(PNTTOVAL(VMALLOCSTR((char*)&add.sin_addr.s_addr,4)));
		VPUSH(VCALLSTACKGET(sys_start,SYS_CBUDP));
		if (VSTACKGET(0)!=NIL) interpGo();
		else { VPULL();VPULL();VPULL();}
		VPULL();
		return 1;
	}
	return 0;
}
