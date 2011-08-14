//-------------------
// MV
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 07/01/2003
// Derniere mise a jour : 07/01/2003
//

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "param.h"
#include "prodbuffer.h"


Prodbuffer::Prodbuffer()
{
	size=PRODBUFFER_FIRSTSIZE;
	buffer=new char[size];
	index=0;
};

Prodbuffer::~Prodbuffer()
{
	delete buffer;
};


// initialiation du buffer (à appeler au début de toute nouvelle production)
void Prodbuffer::reinit()
{
	index=0;
}

void Prodbuffer::bigger()
{
	char* newbuf=new char[size*2];
	memcpy(newbuf,buffer,size);
	delete buffer;
	size*=2;
	buffer=newbuf;
}

// ajout d'un caractère en fin de buffer
void Prodbuffer::addchar(char c)
{
	if (index>=size) bigger();
	buffer[index++]=c;
}

// printf à la suite du buffer
void Prodbuffer::printf(const char *format, ...)
{
	va_list arglist;
	int sizeout;

	va_start(arglist,format);
	//while(((sizeout=vsnprintf(buffer+index,size-index-1,format,arglist))<0)||(sizeout>=size-index-1)) bigger();
	va_end(arglist);
	index+=sizeout;
}

// ajout d'un entier en fin de buffer
void Prodbuffer::addint(int i)
{
	addchar(i); i>>=8;
	addchar(i); i>>=8;
	addchar(i); i>>=8;
	addchar(i);
}

// ajout d'un short en fin de buffer
void Prodbuffer::addshort(int i)
{
	addchar(i); i>>=8;
	addchar(i);
}

// ajout d'une chaîne
void Prodbuffer::addstr(char *src,int len)
{
//	addint(len);
	int i; for(i=0;i<len;i++) addchar(*(src++));
}

// modification d'un caractère dans le buffer
void Prodbuffer::setchar(int ind,char c)
{
	buffer[ind]=c;
}

// modification d'un entier dans le buffer
void Prodbuffer::setint(int ind,int i)
{
	setchar(ind,i); i>>=8;
	setchar(ind+1,i); i>>=8;
	setchar(ind+2,i); i>>=8;
	setchar(ind+3,i);
}

// modification d'un short dans le buffer
void Prodbuffer::setshort(int ind,int i)
{
	setchar(ind,i); i>>=8;
	setchar(ind+1,i);
}

// diminue la taille actuelle
void Prodbuffer::cut(int len)
{
	if ((len>=0)&&(len<index)) index=len;
}

// retourne la taille actuelle
int Prodbuffer::getsize()
{
	return index;
}

// retourne un pointeur vers le début du buffer
char* Prodbuffer::getstart()
{
	return buffer;
}
