//-------------------
// MV
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 07/01/2003
// Derniere mise a jour : 07/01/2003
//

#include "resource.h"

#include "param.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filesystem.h"
#include "file.h"
#include "terminal.h"
#include "memory.h"
#include <stdarg.h>


Terminal::Terminal(void)
{
  m=NULL;
  sizeinternalbuffer=64;
  internalbuffer=new char[sizeinternalbuffer];
}

Terminal::~Terminal()
{

}


void Terminal::echo(int msk,char *s)
{
	if (s)
	{
		if (msk&LOG_USER) fprintf(stdout,"%s",s);
		else fprintf(stderr,"%s",s);
	}
}

void Terminal::echo(int msk,char* s,int size)
{
	if (size+1>sizeinternalbuffer)
	{
		delete internalbuffer;
		sizeinternalbuffer=size+1;
		internalbuffer=new char[sizeinternalbuffer];
	}
	memcpy(internalbuffer,s,size);
	internalbuffer[size]=0;
	echo(msk,internalbuffer);
}

void Terminal::printf(int msk,const char *format, ...)
{
	va_list arglist;
	int sizeout;
	va_start(arglist,format);
	while(((sizeout=vsnprintf(internalbuffer,sizeinternalbuffer-1,format,arglist))<0)||(sizeout>=sizeinternalbuffer-1))
	{
		delete internalbuffer;
		sizeinternalbuffer*=2;
		internalbuffer=new char[sizeinternalbuffer];
	}
	va_end(arglist);
	echo(msk,internalbuffer);
}


