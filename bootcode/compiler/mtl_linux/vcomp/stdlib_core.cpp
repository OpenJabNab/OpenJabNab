//-------------------
// MV
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 07/01/2003
// Derniere mise a jour : 07/01/2003
//

#include "param.h"
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>


#include "terminal.h"
#include "prodbuffer.h"
#include "memory.h"
#include "util.h"
#include "interpreter.h"
#include "parser.h"
#include "compiler.h"


// types de base
#define NBcore 115 /******** A ACTUALISER! ************/
const char* corename[]=
{NULL,NULL,NULL,NULL,NULL,"hd","tl","Secholn","Secho","Iecholn",					//10
"Iecho","time_ms","tabnew","abs","min","max","rand","srand",					//8
"time","strnew","strset","strcpy","vstrcmp","strfind","strfindrev","strlen",		//8
"strget","strsub","strcat","tablen","strcatlist","led","motorset","motorget",	//8
"fixarg1","fixarg2","fixarg3","fixarg4","fixarg5","fixarg6","fixarg7","fixarg8",	//8
"button2","button3","playStart","playFeed","playStop","recStart","recStop","recVol",	//8
"load","gc","save","bytecode","loopcb",	// 5
"udpStart","udpCb","udpStop","udpSend","tcpOpen","tcpClose","tcpSend","tcpCb","tcpListen","tcpEnable",	//10
"envget","envset","sndVol","rfidGet","playTime","sndRefresh","sndWrite","sndRead","sndFeed","sndAmpli",		//10
"netCb","netSend","netState","netMac","netChk","netSetmode","netScan","netAuth","netPmk","netRssi",	//10
"netSeqAdd","strgetword","strputword","atoi","htoi","itoa","ctoa","itoh",	// 8
"ctoh","itobin2","listswitch","listswitchstr","corePP","corePush","corePull","coreBit0", //8
"reboot","flashFirmware","strcmp","adp2wav","wav2adp","alaw2wav","wav2alaw",	//7
"crypt","uncrypt","rfidGetList","rfidRead","rfidWrite", // 5
"i2cRead", "i2cWrite" // 2
};



int coreval[]=
{0,0,0,0,0,OPhd,OPtl,OPSecholn,OPSecho,OPIecholn,
OPIecho,OPtime_ms,OPtabnew,OPabs,OPmin,OPmax,OPrand,OPsrand,
OPtime,OPstrnew,OPstrset,OPstrcpy,OPvstrcmp,OPstrfind,OPstrfindrev,OPstrlen,
OPstrget,OPstrsub,OPstrcat,OPtablen,OPstrcatlist,OPled,OPmotorset,OPmotorget,
OPfixarg,OPfixarg,OPfixarg,OPfixarg,OPfixarg,OPfixarg,OPfixarg,OPfixarg,
OPbutton2,OPbutton3,OPplayStart,OPplayFeed,OPplayStop,OPrecStart,OPrecStop,OPrecVol,
OPload,OPgc,OPsave,OPbytecode,OPloopcb,
OPudpStart,OPudpCb,OPudpStop,OPudpSend,OPtcpOpen,OPtcpClose,OPtcpSend,OPtcpCb,OPtcpListen,OPtcpEnable,
OPenvget,OPenvset,OPsndVol,OPrfidGet,OPplayTime,OPsndRefresh,OPsndWrite,OPsndRead,OPsndFeed,OPsndAmpli,
OPnetCb,OPnetSend,OPnetState,OPnetMac,OPnetChk,OPnetSetmode,OPnetScan,OPnetAuth,OPnetPmk,OPnetRssi,
OPnetSeqAdd,OPstrgetword,OPstrputword,OPatoi,OPhtoi,OPitoa,OPctoa,OPitoh,
OPctoh,OPitobin2,OPlistswitch,OPlistswitchstr,OPcorePP,OPcorePush,OPcorePull,OPcoreBit0,
OPreboot,OPflashFirmware,OPstrcmp,OPadp2wav,OPwav2adp,OPalaw2wav,OPwav2alaw,
OPcrypt,OPuncrypt,OPrfidGetList,OPrfidRead,OPrfidWrite,
OPi2cRead, OPi2cWrite
};

int corecode[]=
{CODE_TYPE,CODE_TYPE,CODE_TYPE,CODE_TYPE,CODE_TYPE,1,1,1,1,1,
1,0,2,1,2,2,0,1,
0,1,3,5,5,5,5,1,
2,3,2,1,1,2,2,1,
2,2,2,2,2,2,2,2,
0,0,2,3,0,3,0,2,
5,0,5,1,1,
1,1,1,6,2,1,4,1,1,2,
0,1,1,0,0,0,2,1,3,1,
1,6,0,0,4,3,1,4,2,0,
2,2,3,1,1,1,1,1,
1,1,2,2,0,1,1,2,
2,3,2,5,5,6,6,
5,5,0,2,3,
2,3
};

const char* coretype[]=
{"I","S","F","Env","Xml","fun[list u0]u0","fun[list u0]list u0","fun[S]S","fun[S]S","fun[u0]u0",
"fun[u0]u0","fun[]I","fun[u0 I]tab u0","fun[I]I","fun[I I]I","fun[I I]I","fun[]I","fun[I]I",
"fun[]I","fun[I]S","fun[S I I]S","fun[S I S I I]S","fun[S I S I I]I","fun[S I S I I]I","fun[S I S I I]I","fun[S]I",
"fun[S I]I","fun[S I I]S","fun[S S]S","fun[tab u0]I","fun[list S]S","fun[I I]I","fun[I I]I","fun[I]I",
"fun[fun[u0]u100 u0]fun[]u100",
 "fun[fun[u1 u0]u100 u0]fun[u1]u100",
 "fun[fun[u1 u2 u0]u100 u0]fun[u1 u2]u100",
 "fun[fun[u1 u2 u3 u0]u100 u0]fun[u1 u2 u3]u100",
 "fun[fun[u1 u2 u3 u4 u0]u100 u0]fun[u1 u2 u3 u4]u100",
 "fun[fun[u1 u2 u3 u4 u5 u0]u100 u0]fun[u1 u2 u3 u4 u5]u100",
 "fun[fun[u1 u2 u3 u4 u5 u6 u0]u100 u0]fun[u1 u2 u3 u4 u5 u6]u100",
 "fun[fun[u1 u2 u3 u4 u5 u6 u7 u0]u100 u0]fun[u1 u2 u3 u4 u5 u6 u7]u100",
 "fun[]I","fun[]I","fun[I fun[I]I]I","fun[S I I]I","fun[]I","fun[I I fun[S]I]I","fun[]I","fun[S I]I",
 "fun[S I S I I]I","fun[]I","fun[S I S I I]I","fun[S]S","fun[fun[]I]fun[]I",
 "fun[I]I","fun[fun[I S S]I]fun[I S S]I","fun[I]I","fun[I S I S I I]I","fun[S I]I","fun[I]I","fun[I S I I]I","fun[fun[I I S]I]fun[I I S]I","fun[I]I","fun[I I]I",
 "fun[]S","fun[S]S","fun[I]I","fun[]S","fun[]I","fun[]I","fun[I I]I","fun[I]I","fun[S I I]I","fun[I]I",
 "fun[fun[S S]I]fun[S S]I","fun[S I I S I I]I","fun[]I","fun[]S","fun[S I I I]I","fun[I S I]I","fun[S]list[S S S I I I I]","fun[[S S S I I I I] I I S][S S S I I I I]","fun[S S]S","fun[]I",
 "fun[S I]S","fun[S I]I","fun[S I I]S","fun[S]I","fun[S]I","fun[I]S","fun[I]S","fun[I]S",
 "fun[I]S","fun[I]S","fun[list[u0 u1] u0]u1","fun[list[S u1] S]u1","fun[]I","fun[u0]u0","fun[I]I","fun[u0 I]u0",
 "fun[I I]I","fun[S I I]I","fun[S S]I","fun[S I S I I]S","fun[S I S I I]S","fun[S I S I I I]S","fun[S I S I I I]S",
 "fun[S I I I I]I","fun[S I I I I]I","fun[]list S","fun[S I]S","fun[S I S]I",
 "fun[I I]S", "fun[I S I]I"
};


int Compiler::addstdlibcore()
{
	int k;
	if (k=addnative(NBcore,corename,coreval,corecode,coretype,m)) return k;
/*	FILE* f=fopen("bc.txt","w");
	for(k=0;k<NBcore;k++)
	{
		if (corename[k]) fprintf(f,"%s%c%s\n",corename[k],9,coretype[k]);
	}
	fclose(f);
*/	return 0;
}
