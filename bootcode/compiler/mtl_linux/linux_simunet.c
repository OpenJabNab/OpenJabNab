#include <string.h>
#include <stdio.h>

#include <resolv.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

#include "vmem.h"
#include "vloader.h"
#include "vinterp.h"

#include "log.h"

#define TCPMAX 128 // nombre max de sockets tcp pouvant être ouvertes en même temps
#define UDPMAX 128 // nombre max de sockets udp pouvant être ouvertes en même temps

int tcp_sock[TCPMAX];	// =-1 -> disponible
int tcp_enable[TCPMAX];
int tcp_listen[TCPMAX]; // si 1, la socket est en listen (et donc une action sur la socket est une connection)
int tcp_writeEventToNotify[TCPMAX]; // si 1, on a envoyé un write event qui n'a pas encore été suivi d'un read ou write

int udp_sock[UDPMAX];
int udp_port[UDPMAX]; // port=0 -> disponible


// déclarations
int inet_addr_bin(char *ip);
int simunetinit(void);
int checkNetworkEvents(void);
int checkTcpEvents(void);
int tcpEventRead(int fd);
int tcpEventWrite(int fd);
int tcpbysock(int s);
int tcpgetfree(void);
int tcpopen(char* dstip,int dstport);
int tcpclose(int i);
void tcpenable(int i,int enable);
int tcpsend(int i,char* msg, int len);
int tcpservercreate(int port);
int udpbyport(int p);
int udpgetfree(void);
int udpbysock(int s);
int udpcreate(int port);
int udpclose(int port);
int udpsend(int localport,char* dstip,int dstport,char* msg, int len);
int checkUdpEvents();
int udpEventRead(int fd);


void helper_write_buffer(char *buf, int res)
{
	if (res <= 0)
		{ my_printf(LOG_SIMUNET, "BUF empty\n"); return ; }

	my_printf(LOG_SIMUNET, "buffer content (len: %d) >>>>>>>>\n", res);

	// rendre la chaine imprimable
	char printable[2048];
	if (res>2048)
		res = 2048;
	memcpy(printable, buf, 2048);
	int i;
	for (i=0; i<res; ++i)
		if (!isprint(printable[i]) && ('\n' != printable[i]))
			printable[i]='.';

	my_printf(LOG_SIMUNET, "%.2048s\n", printable);
	my_printf(LOG_SIMUNET, "<<<<<<< end of buffer content\n");
}



/**
	 Cast parce qu'on nous donne les ip dans un format bizarre
 */
int inet_addr_bin(char *ip)
{
	return *(int*)ip;
}


/**
	 Initialiise les structures qui serviront à gérer les fonctions réseau
 */
int simunetinit(void)
{
	;
	memset(&tcp_sock, -1, sizeof(tcp_sock));
	memset(&tcp_enable, 0, sizeof(tcp_enable));
	memset(&tcp_listen, 0, sizeof(tcp_listen));
	memset(&tcp_writeEventToNotify, 0, sizeof(tcp_writeEventToNotify));
	memset(&udp_sock, 0, sizeof(udp_sock));
	memset(&udp_port, 0, sizeof(udp_port));

	return 0;
}

int checkTcpEvents(void);

/**
	 Vérifie les évènements qui se sont produits sur les socket ouvertes et
	 fait les actions appropriées.

	 TODO peut être possible de retravailler les autres fonctions pour
	 avoir les tableaux et valeurs prêtes sans avoir à passer par les
	 boucles ?
 */
int checkNetworkEvents(void)
{
	checkTcpEvents();
	checkUdpEvents();
	return 0;
}

/////////////// TCP /////////////////

int tcpEventRead(int fd);
int tcpEventWrite(int fd);
int tcpbysock(int s);

int checkTcpEvents(void)
{
	fd_set fdset_r, fdset_w, fdset_err;
	int nfds = 0;
	int maxval = 0; // doit pouvoir être dispo directement
	struct timeval timeout = {0, 0} ;
	int i;

	FD_ZERO(&fdset_r);
	FD_ZERO(&fdset_w);
	FD_ZERO(&fdset_err);

	for (i=0; i<TCPMAX; ++i)
		{
			if (tcp_sock[i] != -1)
				{
					FD_SET(tcp_sock[i], &fdset_r);
					FD_SET(tcp_sock[i], &fdset_w);
					FD_SET(tcp_sock[i], &fdset_err);
					if (tcp_sock[i] > maxval)
						maxval = tcp_sock[i];
				}
		}

	int nbevts = select(maxval+1, &fdset_r, NULL /* &fdset_w */, &fdset_err, &timeout);
	if (nbevts < 0)
		{
			my_printf(LOG_SIMUNET, "Sockets : Tcp select failed (%s)\n", strerror(errno));
			return -1;
		}

	/*** write events ***/
	for (i=0; i<TCPMAX; ++i)
		if (tcp_writeEventToNotify[i])
			tcpEventWrite(tcp_sock[i]);

	/*** read events ***/
	if (nbevts > 0)
		{
			for (i=0; i<=maxval; ++i)
				{
					if (FD_ISSET(i, &fdset_w))
						{
							tcpEventWrite(i);
						}
					if (FD_ISSET(i, &fdset_r))
						{
							tcpEventRead(i);
						}
					if (FD_ISSET(i, &fdset_err))
						{
							// TODO
						}
				}
		}

	return 0;
}

/**
	 Gère un evt read sur une socket
 */
int tcpEventRead(int fd)
{
	/* soit une donnée à lire, soit un accept, soit un close */
	/* accept si on l'attend, close si on lit 0 data, read sinon */
	int idx = tcpbysock(fd);

	if (idx < 0)
		{
			// TODO gérer l'erreur
			return 0;
		}

	if (tcp_listen[idx])
		{
			// accept
			struct sockaddr_in cor;
			int ns;
			int sizecor;
			int ni,ip,port;
			char buf[16];

			tcp_listen[idx] = 0;

			sizecor=sizeof(cor);
			ns=accept(fd,(struct sockaddr*)&cor,&sizecor);
			if (ns==-1) return 1;

			ni=tcpgetfree();
			if (ni<0)
				{
					close(ns);
					return 1;
				}
			
			ip=cor.sin_addr.s_addr;
			port=ntohs(cor.sin_port);

			tcp_sock[ni]=ns;

			my_printf(LOG_SIMUNET, "Sockets : accept Tcp from %x:%d (socket=%d)\n",ip,port,ns);
			VPUSH(INTTOVAL(ni));
			VPUSH(INTTOVAL(2));
			sprintf(buf,"%d",idx);
			VPUSH(PNTTOVAL(VMALLOCSTR(buf,strlen(buf))));
			VPUSH(VCALLSTACKGET(sys_start,SYS_CBTCP));
			if (VSTACKGET(0)!=NIL) interpGo();
			else { VPULL();VPULL();VPULL();}
			VPULL();
			return 1;
		}
	// donnée ou close
	else
	{
		char buf[2048];
		int res;
		if (!tcp_enable[idx])
		{
//			Sleep(10);
//			recv(sock,buf,0,0);
//			printf("disabled\n");
			return 1;
		}

		my_printf(LOG_SIMUNET, "Sockets : Read event on %d\n",fd);
		res=recv(fd,buf,2048,0);

		helper_write_buffer(buf, res);

		VPUSH(INTTOVAL(idx));
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
}

/**
	 Gère un evt write sur une socket
 */
int tcpEventWrite(int fd)
{
	int idx = tcpbysock(fd);
	if (idx<0) {
		my_printf(LOG_SIMUNET, "Sockets : idx < 0\n");
		return 0;
	}

	tcp_writeEventToNotify[idx] = 0;

	my_printf(LOG_SIMUNET, "Sockets : Write event on %d\n",idx);
	VPUSH(INTTOVAL(idx));
	VPUSH(INTTOVAL(0));
	VPUSH(NIL);
	VPUSH(VCALLSTACKGET(sys_start,SYS_CBTCP));
	if (VSTACKGET(0)!=NIL) interpGo();
	else { VPULL();VPULL();VPULL();}
	VPULL();
	return 1;
}

/**
	 Retrouve l'indice d'une socket dans le tableau tcp_sock
 */
int tcpbysock(int s)
{
	int i;
	for(i=0;i<TCPMAX;i++) if (tcp_sock[i]==s) return i;
	return -1;
}

/**
	 Renvoie l'indice du premier emplacement libre (socket pas en cours
	 d'utilisation) du tableau tcp_sock
 */
int tcpgetfree(void)
{
	int i;
	for(i=0;i<TCPMAX;i++) if (tcp_sock[i]==-1)
	{
		tcp_enable[i]=1;
		tcp_listen[i]=0;
		return i;
	}
	return -1;
}

/**
	 Ouvre une connection tcp

	 dstip: 
 */
int tcpopen(char* dstip,int dstport)
{
	int socktcp;
	struct sockaddr_in ina;
	int opt=1;

	int i=tcpgetfree();
	if (i<0) return i;

	socktcp=socket(AF_INET,SOCK_STREAM,0);
	if (socktcp==-1) return -1;

	ina.sin_family = AF_INET;
	ina.sin_port   = htons((unsigned short)dstport);
	ina.sin_addr.s_addr = inet_addr_bin(dstip);

	fcntl(socktcp, F_SETFD, O_NONBLOCK);

	if (connect(socktcp,(struct sockaddr *)&ina,sizeof(ina))!=0)
		// ici dans le code windows il y a une condition si SOCKETWOULDBLOCK
	{
		my_printf(LOG_SIMUNET, "Sockets : Tcp connect failed (%s)\n", strerror(errno));
		close(socktcp);
		return -1;
	}
	my_printf(LOG_SIMUNET, "Sockets : create Tcp %s:%d (socket=%d)\n",inet_ntoa(ina.sin_addr),dstport,socktcp);

	tcp_sock[i]=socktcp;
	tcp_writeEventToNotify[i] = 1;

	return i;
}

/**
	 Ferme une connection tcp
 */
int tcpclose(int i)
{
	if ((i>=0)&&(i<TCPMAX)&&(tcp_sock[i]!=-1))
	{
		my_printf(LOG_SIMUNET, "Sockets : Tcp close %d\n",tcp_sock[i]);
		close(tcp_sock[i]);
		tcp_sock[i]=-1;
	}
	return i;
}

/**
	 ?
 */
void tcpenable(int i,int enable)
{
	if ((i>=0)&&(i<TCPMAX)&&(tcp_sock[i]!=-1))
	{
		char buf[16];
		if (enable!=tcp_enable[i])
		{
			tcp_enable[i]=enable;
			if (enable)	recv(tcp_sock[i],buf,0,0);
		}
	}
}


/**
	 Envoie des données sur une connection tcp déjà ouverte
 */
int tcpsend(int i,char* msg, int len)
{
	if ((i>=0)&&(i<TCPMAX)&&(tcp_sock[i]!=-1))
	{
		int res=send(tcp_sock[i],msg,len,0);
		helper_write_buffer(msg, len);
		if (res<0)
		{
			my_printf(LOG_SIMUNET, "Sockets: Tcp could not send data (%s)\n", strerror(errno));
			// ici dans le code windows il y a une condition si SOCKETWOULDBLOCK
			res=-1;
		}
		return res;
	}
	return -1;
}

/**
	 Ouvre le port spécifié et attend des connections dessus
 */
int tcpservercreate(int port)
{
	int opt=1;
	struct sockaddr_in ina;
	int socksrv;
	int i=tcpgetfree();
	if (i<0) return i;

	socksrv=socket(AF_INET,SOCK_STREAM,0);
	if (socksrv==-1)
	{
		return -1;
	}
	setsockopt (socksrv, SOL_SOCKET, SO_REUSEADDR,(char*) &opt, sizeof (opt));
	ina.sin_family = AF_INET;
	ina.sin_port   = htons((unsigned short)port);
	ina.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(socksrv,(struct sockaddr*)&ina,sizeof(ina))!=0)
	{
		my_printf(LOG_SIMUNET, "Sockets : Tcp port %d busy (%s)\n",port, strerror(errno));
		close(socksrv);
		return -1;
	}
	if (listen(socksrv,3)!=0)
	{
		my_printf(LOG_SIMUNET, "Sockets : Tcp port %d listen error (%s)\n",port, strerror(errno));
		close(socksrv);
		return -1;
	}
	my_printf(LOG_SIMUNET, "Sockets : create Tcp server :%d (socket=%d) (%s)\n",port,socksrv, strerror(errno));
	tcp_sock[i]=socksrv;
	tcp_listen[i]=1;
	
	return 0;
}


/////////////// UDP /////////////////

int udpbyport(int p)
{
	int i;
	for(i=0;i<UDPMAX;i++) if (udp_port[i]==p) return i;
	return -1;
}

int udpgetfree(void)
{
	return udpbyport(0);
}

int udpbysock(int s)
{
	int i;
	for(i=0;i<UDPMAX;i++) if ((udp_port[i])&&(udp_sock[i]==s)) return i;
	return -1;
}

int udpcreate(int port)
{
	int sockudp;
	struct sockaddr_in ina;
	int opt=1;
	int i;

	udpclose(port);
	
	i=udpgetfree();
	if (i<0) return i;

	sockudp=socket(AF_INET,SOCK_DGRAM,0);
	if (sockudp==-1) return -1;

	ina.sin_family = AF_INET;
	ina.sin_port   = htons((unsigned short)port);
	ina.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockudp,(struct sockaddr*)&ina,sizeof(ina))!=0)
	{
		my_printf(LOG_SIMUNET, "Sockets : Udp port %d busy\n",port);
		close(sockudp);
		return -1;
	}
	setsockopt(sockudp, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof (opt));
	setsockopt(sockudp, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof (opt));
	my_printf(LOG_SIMUNET, "Sockets : create Udp :%d (socket=%d)\n",port,sockudp);
	
	udp_port[i]=port;
	udp_sock[i]=sockudp;
	return i;
}

int udpclose(int port)
{
	int i=udpbyport(port);
	if (i>=0)
	{
		close(udp_sock[i]);
		udp_port[i]=0;
	}
	return i;
}

int udpsend(int localport,char* dstip,int dstport,char* msg, int len)
{
	struct sockaddr_in ina;
	int i;

	i=udpbyport(localport);
	if (i<0) i=udpcreate(localport);
	if (i>=0)
	{
		ina.sin_family = AF_INET;
		ina.sin_port   = htons((unsigned short)dstport);
		ina.sin_addr.s_addr = inet_addr_bin(dstip);

		// hack pour dns. dans le bytecode de simu, on considère que
		// 192.168.1.1 est le dns. Si on essaie d'envoyer un paquet à
		// cette adresse, il faut translater vers le "vrai" dns.
		in_addr_t ip_192_168_1_1 = inet_addr("192.168.1.1");
		if (!memcmp(&(ina.sin_addr),&ip_192_168_1_1, sizeof(ip_192_168_1_1))) {
			my_printf(LOG_SIMUNET, "Sockets : (hack) converting 192.168.1.1 to real dns ip\n");
			res_init();
			if (_res.nscount <= 0) { my_printf(LOG_SIMUNET, "Fatal error: no DNS available. Should abort\n"); return 0; };
			ina.sin_addr = _res.nsaddr_list[0].sin_addr;
		}

		my_printf(LOG_SIMUNET, "Sockets : send %d bytes on chn %d\n",len,i);
		sendto(udp_sock[i],msg,len,0,(struct sockaddr *)&ina,sizeof(ina));
		return 0;
	}
	return -1;
}



int checkUdpEvents(void)
{
	fd_set fdset_r, fdset_w, fdset_err;
	int nfds = 0;
	int maxval = 0; // doit pouvoir être dispo directement
	struct timeval timeout = {0, 0} ;
	int i;

	FD_ZERO(&fdset_r);
	FD_ZERO(&fdset_w);
	FD_ZERO(&fdset_err);

	for (i=0; i<UDPMAX; ++i)
		{
			if (udp_sock[i] != -1)
				{
					FD_SET(udp_sock[i], &fdset_r);
/* 					FD_SET(udp_sock[i], &fdset_w); */
/* 					FD_SET(udp_sock[i], &fdset_err); */
					if (udp_sock[i] > maxval)
						maxval = udp_sock[i];
				}
		}

	int nbevts = select(maxval+1, &fdset_r, &fdset_w, &fdset_err, &timeout);

	if (nbevts < 0)
		{
			my_printf(LOG_SIMUNET, "Sockets : Udp select failed (%s)\n", strerror(errno));
			return -1;
		}

	if (nbevts > 0)
		{
			for (i=0; i<=maxval; ++i)
				{
					if (FD_ISSET(i, &fdset_r))
						{
							udpEventRead(i);
						}
				}
		}

	return 0;
}

int udpEventRead(int fd)
{
	char buf[4096];
	struct sockaddr_in add;
	int i=udpbysock(fd);
	int l=sizeof(add);
	int res=recvfrom(fd,buf,4096,0,(struct sockaddr *)&add,&l);
	if (res<0) return 1;

	my_printf(LOG_SIMUNET, "Sockets : UDP Read %d bytes on :%d from %s:%d\n",res,udp_port[i],inet_ntoa(add.sin_addr),ntohs(add.sin_port));
	VPUSH(INTTOVAL(i));
	VPUSH(PNTTOVAL(VMALLOCSTR(buf,res)));
	VPUSH(PNTTOVAL(VMALLOCSTR((char*)&add.sin_addr.s_addr,4)));
	VPUSH(VCALLSTACKGET(sys_start,SYS_CBUDP));
	if (VSTACKGET(0)!=NIL) interpGo();
	else { VPULL();VPULL();VPULL();}
	VPULL();
	return 1;
}
