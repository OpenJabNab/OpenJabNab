//-------------------
// Moteur 3d
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 03/09/2002
// Derniere mise a jour : 05/10/2002
//



#include <stdio.h>
#include <string.h>

#include "param.h"
#include "filesystem.h"
#include "file.h"
#include "parser.h"
#include "terminal.h"
#include "memory.h"

// test de lettre
int isletter(int c)
{
	if ((c>='A')&&(c<='Z')) return 1;
	if ((c>='a')&&(c<='z')) return 1;
	if (c=='_') return 1;
	return 0;
}
// test de chiffre decimal
int isnum(int c)
{
	if ((c>='0')&&(c<='9')) return 1;
	return 0;
}
// test de lettre
int ishex(int c)
{
	if ((c>='0')&&(c<='9')) return 1;
	if ((c>='A')&&(c<='F')) return 1;
	if ((c>='a')&&(c<='f')) return 1;
	return 0;
}

// test de lettre ou de chiffre
int isletnum(int c)
{
	if ((c>='A')&&(c<='Z')) return 1;
	if ((c>='a')&&(c<='z')) return 1;
	if ((c>='0')&&(c<='9')) return 1;
	if (c=='_') return 1;
	return 0;
}

// test si un mot est un label (une lettre puis des lettres ou des chiffres)
int islabel(char* src)
{
	if (!isletter(*src++)) return 0;
	while(*src)
	{
		if (!isletnum(*src++)) return 0;
	}
	return 1;
}

// test si un mot est un nombre décimal (que des chiffres)
int isdecimal(char* src)
{
	while(*src)
	{
		if (!isnum(*src++)) return 0;
	}
	return 1;
}

// test si un mot est un nombre hexadécimal (que des chiffres)
int ishexadecimal(char* src)
{
	while(*src)
	{
		if (!ishex(*src++)) return 0;
	}
	return 1;
}

// test si un mot est un nombre flottant (que des chiffres avec un point)
int isfloat(char* src)
{
	int debut=1;
	int point=0;
	while(*src)
	{
		if ((*src=='.')&&(!debut)&&(!point)) point=1;
		else if (!isnum(*src)) return 0;
		src++;
		debut=0;
	}
	return point;
}

// retourne le code ascii correspondant à un chiffre hexadécimal
int ctoh(int c)
{
	c&=15;
	if (c<10) return '0'+c;
	return 'a'+c-10;
}

// retourne le chiffre correspondant à un code ascii hexadécimal
int htoc(int c)
{
	if ((c>='0')&&(c<='9')) return c-'0';
	else if ((c>='A')&&(c<='F')) return c-'A'+10;
	else if ((c>='a')&&(c<='f')) return c-'a'+10;
	return 0;
}

// lecture d'une chaîne décimale (s'arrête au premier caractère incorrect)
int mtl_atoi(char* src)
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
int mtl_htoi(char* src)
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

// lecture d'une chaîne flottante (s'arrête au premier caractère incorrect)
mtl_float mtl_atof(char* src)
{
	int c;
	mtl_float f=0;
	mtl_float div=1;
	int point=0;
	while(c=*src++)
	{
		if ((c=='.')&&(!point)) point=1;
		else if ((c>='0')&&(c<='9'))
		{
			f=(f*10)+c-'0';
			if (point) div*=10;
		}
		else return f/div;
	}
	return f/div;
}

Parser::Parser(Terminal* t,FileSystem* fs,const char *filename)
{
	term=t;

	srcname=new char[strlen(filename)+1];
	strcpy(srcname,filename);

	File* file=new File(fs);
	int size;

	src=NULL;
	if (!file->openread(filename)) src=file->getcontent(&size);
	delete file;
	if (!src)
	{
		t->printf(LOG_WARNING,"Parser : cannot open file %s\n",filename);
		src=new char[1];
		src[0]=0;
	}
	index=0;

	indexsavedchar=-1;

	again=0;

	lign=offligne=0;
	lign0=offligne0=offchar0=0;

	token=NULL;
}

Parser::Parser(Terminal* t,const char *buf)
{
	srcname=NULL;
	term=t;
	src=new char[strlen(buf)+1];
	strcpy(src,buf);
	index=0;

	indexsavedchar=-1;

	again=0;

	lign=offligne=0;
	lign0=offligne0=offchar0=0;

	token=NULL;
}


Parser::~Parser()
{
	delete src;
	if (srcname) delete srcname;
}

void Parser::giveback()
{
	again=1;
}

char* Parser::next(int filter)
{
	if (again)
	{
		again=0;
		return token;
	}

	int i;
	while(1)
    {
		int k;
		do
		{
			k=gettoken(filter);
		} while (k==-2);
		if (k)
		{
			token=NULL;
			return NULL;
		}
//		term->printf(0,"token=%s\n",token);
		if (strcmp(token,"/*")) return token;	// ok
		i=1;	// gestion des commentaires
		while(i)
        {
			if (gettoken(filter)==-1)
			{
				term->printf(LOG_COMPILER,"unbalanced comment reaches EOF\n");
				token=NULL;
				return NULL;
			}
			if (!strcmp(token,"/*")) i++;
			if (!strcmp(token,"*/")) i--;
        }
    }
}

void Parser::savechar(int i)
{
	indexsavedchar=i;
	savedchar=src[i];
	src[i]=0;
}

// demande de relire le dernier caractère
void Parser::againchar()
{
	index--;
	if (src[index]==10) lign--;
}

// lit le prochain caractère
int Parser::nextchar()
{
	int c;

	c=src[index];
	if (c)
	{
		index++;
		if (c==10)
		{
			offligne=index;
			lign++;
		}
	}
	return c;
}


void Parser::restorechar()
{
	// on restaure le caractère escamoté à la fin du token précédent
	if (indexsavedchar!=-1)
	{
		src[indexsavedchar]=savedchar;
		indexsavedchar=-1;
	}
}

int Parser::gettoken(int filter)
{
	int c,d,f;

	restorechar();

	do
	{   // recherche le début du token
		// on sauvegarde la position dans le source
		lign0=lign;
		offligne0=offligne;
		offchar0=index;
		c=nextchar();
		if (!c) return -1;	// fin du fichier, pas de nouveau token
	} while(c<=32);

	token=&src[offchar0];

	f=0;
	if (c=='\"')	// token chaîne de caractères
		while(1)
		{
			c=nextchar();
			if (!c)
			{
				term->printf(LOG_COMPILER,"uncomplete string reaches EOF\n");
				return -1;
			}
			if ((c=='\"')&&(f==0))
			{
				savechar(index);
				return 0;
			}
			if (c=='\\') f=1-f;
			else f=0;
		}
	if (isletnum(c))	// token nombre ou label
	{
		int onlynum=1;
		while(1)
		{
			if (!isnum(c)) onlynum=0;
			c=nextchar();
			if (!c) return 0;
			if ((c=='.')&&(onlynum))	// nombre flottant
			{
				while(1)
				{
					c=nextchar();
					if (!c) return 0;
					if (!isnum(c))
					{
						againchar();
						savechar(index);
						return 0;
					}
				}
			}
			if (!isletnum(c))
			{
				againchar();
				savechar(index);
				return 0;
			}
		}
	}
	d=nextchar();
	if (!d)	return 0;	// fin du fichier sur un caractère spécial

	if ( ((c=='&')&&(d=='&'))
		||((c=='|')&&(d=='|'))
		||((c==':')&&(d==':'))
		||((c=='^')&&(d=='^'))
		||((c==';')&&(d==';'))
		||((c=='-')&&(d=='>'))
		||((c=='<')&&((d=='<')||(d=='/')))
		||((c=='>')&&(d=='>'))
		||((c=='=')&&(d=='='))
		||((c=='/')&&((d=='*')||(d=='>')))
		||((c=='*')&&(d=='/'))
		)
	{
		// caractère double reconnu
	}
	else if ((c=='/')&&(d=='/'))
	{
		do	// cas du commentaire //
		{
			c=nextchar();
			if (c==10) return -2;
		} while (c);
		return -1;	// fin de fichier, pas de nouveau token
	}
	else if (((c=='!')||(c=='>')||(c=='<'))&&(d=='='))
	{
/*		d=nextchar();
		if (!d)	return 0; // fin du fichier sur un caractère spécial composé
		if (d!='.') againchar();
*/	}
	else
		againchar();
	savechar(index);
	return 0;
}


void Parser::echoposition()
{
	restorechar();
	if (srcname) term->printf(LOG_COMPILER,">line %d in '%s' :\n>",lign0+1,srcname);
	else term->printf(LOG_COMPILER,">line %d :\n>",lign0+1);
	char* p=&src[offligne0];
	int i=0;
	while((p[i])&&(p[i]!='\n')) i++;
	term->echo(LOG_COMPILER,p,i);
	term->printf(LOG_COMPILER,"\n>");
	for(i=0;i<offchar0-offligne0;i++) { term->printf(LOG_COMPILER,  "%c", (p[i]!='\t') ? ' ' : '\t'); }
	term->printf(LOG_COMPILER,"^\n");
}

