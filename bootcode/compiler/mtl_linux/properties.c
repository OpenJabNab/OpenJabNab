// VLISP Virtual Machine - 2006 - by Sylvain Huet
// Lowcost IS Powerfull

//#include"vlow.h"
#include <stdlib.h>
#include <string.h>
#include<stdio.h>
#include"vmem.h"
#include"properties.h"
#include "log.h"

struct Prop
{
	char* name;
	char* val;
	struct Prop* nxt;
};
typedef struct Prop* prop;

char* emptystring="";
struct Prop *Properties;

// retourne le contenu d'un fichier dans un buffer
// il ne faudra pas oublier de désalloué le buffer, par un appel à fileReleaseBuffer
char* fileLoadL(const char *filename,int *length)
{
	FILE* file;
	int size;
	char *buffer;

	file=fopen(filename,"rb");
	if (!file) return NULL;

	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);

	buffer=(char*)malloc(size+1);
	size=fread((void*)buffer,1,size,file);
	buffer[size]=0;
	fclose(file);
	if (length) *length=size;
	return buffer;
}

char* fileLoad(const char *filename)
{
	return fileLoadL(filename,NULL);
}

// libère le buffer retourné par fileLoad
void fileReleaseBuffer(char* buffer)
{
	if (buffer) free((void*)buffer);
}

// lecture d'une chaîne hexadécimale (s'arrête au premier caractère incorrect)
int htoi(const char* src)
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


// alloue une chaîne
char* stringStore(const char *src,int len)
{
	char* s=(char*)malloc(len+1);
	memcpy(s,src,len);
	s[len]=0;
	return s;
}

// désalloue une chaîne
void stringRelease(char *src)
{
	if (src) free((void*)src);
}


// fonction d'identification du premier mot d'une chaîne
// ignore les espaces initiaux, et s'arrête au premier espace ou caractère spécial rencontré
void suppspace(char* begin,int len, char **wordbegin, int *wordlen)
{
	int i=0;
	int j;

	while((i<len)&&(begin[i]>0)&&(begin[i]<=32)) i++;	// suppression des espaces
	*wordbegin=begin+i;

	j=i;
	while((j<len)&&((begin[j]>32)||(begin[j]<0))) j++;	// aller jusqu'au bout du tag
	*wordlen=j-i;
}


// allocation de mémoire pour la structure prop
prop propAlloc()
{
	prop pnt=(prop)malloc(sizeof(struct Prop));
	return pnt;
}

// désallocation de mémoire pour la structure prop
void propFree(prop pnt)
{
	if (pnt) free((void*)pnt);
}

// détruit une prop, avec tous ses champs
void propRelease(prop pnt)
{
	if (pnt)
	{
		propRelease(pnt->nxt);
		stringRelease(pnt->name);
		stringRelease(pnt->val);
		propFree(pnt);
	}
}

// ajout d'un champ à une structure prop 'nxt'
prop propAdd(const char *name,int namelen,const char *val,int vallen,prop nxt)
{
	char *sname,*sval;

	if (name[0] == ';')
		return nxt;

	prop pnt=propAlloc();
	sname=stringStore(name,namelen);
	sval=stringStore(val,vallen);

	pnt->name=sname;
	pnt->val=sval;
	pnt->nxt=nxt;
	return pnt;
}

// inverse la liste de propriétés
prop propReverse(prop pnt)
{
	prop last=NULL;
	prop nxt=NULL;
	while(pnt)
	{
		nxt=pnt->nxt;
		pnt->nxt=last;
		if (!nxt) return pnt;
		last=pnt;
		pnt=nxt;
	}
	return NULL;
}

// parsing d'un buffer rempli avec le contenu d'un fichier de prop
prop propParseInv(char *src)
{
	prop pnt=NULL;	// pointeur vers la property en construction
	int i,name_b,name_e,val_b,val_e;

	i=0;
	while(src[i])
	{
		while((src[i])&&(src[i]>0)&&(src[i]<=32)) i++;
		if (!src[i]) return pnt;
		name_b=i;
		while(src[i]>32) i++;
		if (!src[i]) return pnt;
		name_e=i;
		while((src[i])&&((src[i]==32)||(src[i]==9))) i++;
		if (!src[i]) return pnt;
		if (src[i]>32)	// il y a bien un argument
		{
			val_b=i;
			while((src[i]>=32)||(src[i]<0)) i++;
			val_e=i;
			pnt=propAdd(src+name_b,name_e-name_b,src+val_b,val_e-val_b,pnt);
		}
		else pnt=propAdd(src+name_b,name_e-name_b,src,0,pnt);
	}
	return pnt;
}

// parsing d'un buffer rempli avec le contenu d'un fichier de prop
prop propParse(char *src)
{
	return propReverse(propParseInv(src));
}

// retourne le premier élément
prop propFirst(prop pnt)
{
	return pnt;
}

// retourne l'élément suivant
prop propNext(prop pnt)
{
	return pnt->nxt;
}

// charge un fichier de prop
// retourne un pointeur vers la structure prop du dernier champ
prop propLoad(const char *file)
{
	prop pnt=NULL;	// pointeur vers la property en construction
	char* src=fileLoad(file);

	if (!src) return NULL;
	pnt=propParse(src);
	fileReleaseBuffer(src);
	return pnt;
}


// dump d'une prop sur la sortie standard
void propDump(prop pnt)
{
	while(pnt)
	{
		my_printf(LOG_INIT, "%s := %s\n",pnt->name,pnt->val);
		pnt=pnt->nxt;
	}
}


// retourne la valeur d'un champ
char* propGet(prop pnt,const char *field)
{
	while(pnt)
	{
		if (!strcmp(pnt->name,field)) return pnt->val;
		pnt=pnt->nxt;
	}
	return emptystring;
}

// retourne la valeur entière d'un champ
int propGetInt(prop pnt,const char *field)
{
	char *p=propGet(pnt,field);
	if ((p[0]=='0')&&((p[1]=='x')||(p[1]=='X'))) return htoi(p+2);
	return atoi(p);
}

int PropLoad(const char* file)
{
	Properties=propLoad(file);
	return 0;
}

// retourne la valeur d'un champ
char* PropGet(const char *field)
{
	return propGet(Properties,field);
}

// retourne la valeur entière d'un champ
int PropGetInt(const char *field)
{
	return propGetInt(Properties,field);
}

// efface la valeur d'un champ s'il est présent, chaine
void PropSet(const char *field, const char *val)
{
	prop tmp = Properties;

	while (tmp && strcmp(tmp->name, field))
		tmp = tmp->nxt;
	if (!tmp)
		Properties = propAdd(field,strlen(field),val,strlen(val),Properties);
	else
		{
			stringRelease(tmp->val);
			tmp->val = stringStore(val, strlen(val));
		}
}

// affiche toutes les prop sur la sortie standard
void PropDump()
{
	propDump(Properties);
}
