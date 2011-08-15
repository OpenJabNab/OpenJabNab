//-------------------
// MV
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 07/01/2003
// Derniere mise a jour : 07/01/2003
//

#include <stdio.h>
#include <string.h>

#include "param.h"
#include "terminal.h"
#include "memory.h"
#include "util.h"
#include "interpreter.h"

#define GCincPeriod 20


// Gestion de la pile
Stack::Stack()
{
	base=NULL;
}
Stack::~Stack()
{
	if (base) delete base;
}

void Stack::dump(FILE *f)
{
	const char* buf="stck";
	fwrite((void*)buf,1,4,f);
	fwrite((void*)&size,1,4,f);
	fwrite((void*)&pp,1,4,f);
	fwrite((void*)&base,1,4,f);
	fwrite((void*)base,1,4*size,f);
}

void Stack::initialize(int s)
{
	size=s;
	base=new int[size];
	pp=base;
}

int Stack::bigger(Memory* m)
{
	int currentpp=pp-base;

	size*=2;

	int* newbase=new int[size];
	if (!newbase)
	{
		PRINTF(m)(LOG_RUNTIME,"Stack : out of Stack Memory\n");
		AbortMetal(m,1);
		return MTLERR_OM;	// impossible d'augmenter la taille de la pile
	}
	int i; for(i=0;i<=currentpp;i++) newbase[i]=base[i];
	delete base;
	base=newbase;
	pp=&base[currentpp];
	return 0;
}




// ajout d'une racine
int Memory::addroot(int *p)
{
	int k;

	if (k=push(PNTTOVAL(p))) return k;
	int* r=malloc(LIST_LENGTH,TYPE_TAB);
	TABSET(this,r,LIST_VAL,PNTTOVAL(p));
	TABSET(this,r,LIST_NEXT,root);
	root=PNTTOVAL(r);
	STACKDROP(this);
	return 0;
}

// suppression d'une racine
void Memory::removeroot(int *p)
{
	int* last=NULL;	// pointeur vers le précédent maillon de la liste
	int vq=root;
	while(vq!=NIL)
	{
		int* q=VALTOPNT(vq);
		if (TABGET(q,LIST_VAL)==PNTTOVAL(p))
		{
			if (last) TABSET(this,last,LIST_NEXT,TABGET(q,LIST_NEXT));
			else root=TABGET(q,LIST_NEXT);
			return;
		}
		last=q;
		vq=TABGET(q,LIST_NEXT);
	}
}

Memory::Memory(int size,Terminal *t,FileSystem *fs)
{
	term=t;
	filesystem=fs;
	size0=size;	// on retient la taille initiale, elle sert d'ordre de grandeur
	gcincperiod=GCincPeriod;
	abort=0;

	util=new Util(this);
	winutil=NULL;
//	listing();
}

Memory::~Memory()
{
//	if (winutil) delete winutil;
	delete util;
}

void Memory::stop()
{
	abort=1;
	util->stop();
	root=NIL;
	stack.pp=0;
}

int Memory::start()
{
	root=NIL;

	stack.initialize(STACK_FIRST_SIZE);

	PRINTF(this)(LOG_RUNTIME,"Metal Virtual Machine\n");
	PRINTF(this)(LOG_RUNTIME,"V0.2 - Sylvain Huet - 2005\n");
	PRINTF(this)(LOG_RUNTIME,"--------------------------\n");
	return util->start();
}

void Memory::dump()
{
}


int* Memory::malloc(int size,int type)
{
	int *p=NULL;

	int blocsize=size+HEADER_LENGTH;
	p=new int[blocsize];
	if (!p) return p;

	HEADERSETSIZETYPE(p,blocsize,type);

	return p;
}



int* Memory::mallocClear(int size)
{
	int* p=malloc(size,TYPE_TAB);
	if (!p) return p;
	int i;
	for(i=0;i<size;i++) TABSET(this,p,i,NIL);
	return p;
}

// allocation de type TYPE_EXT (la fonction fun(pnt) sera appelée lors de l'oubli du bloc)
int* Memory::mallocExternal(void* pnt,FORGET fun)
{
	int* p=malloc(2,TYPE_EXT);
	if (!p) return p;
	TABSET(this,p,0,(int)pnt);
	TABSET(this,p,1,(int)fun);
	return p;
}

// allocation de type TYPE_EXT (la fonction fun(pnt) sera appelée lors de l'oubli du bloc)
int Memory::pushExternal(void* pnt,FORGET fun)
{
	int* p=mallocExternal(pnt,fun);
	if (!p) return MTLERR_OM;
	return push(PNTTOVAL(p));
}

int* Memory::storenosrc(int size)
{
	// calcul de la taille d'un bloc pouvant contenir une certain nombre de caractères
	// il faut 1 mot pour la taille et un octet nul final
	int l=2+(size>>2);

	int* p=malloc(l,TYPE_BINARY);
	if (!p) return p;
	STRSETLEN(p,size);
	STRSTART(p)[size]=0;
	return p;
}

int* Memory::storebinary(const char *src,int size)
{
	// calcul de la taille d'un bloc pouvant contenir une certain nombre de caractères
	// il faut 1 mot pour la taille et un octet nul final
	int l=2+(size>>2);

	int* p=malloc(l,TYPE_BINARY);
	if (!p) return p;
	STRSETLEN(p,size);
	memcpy(STRSTART(p),src,size);
	STRSTART(p)[size]=0;
	return p;
}

int* Memory::storestring(const char *src)
{
	return storebinary(src,strlen(src));
}

int Memory::deftab(int size)
{
	int* p=malloc(size,TYPE_TAB);
	if (!p) return MTLERR_OM;
	int i; for(i=size-1;i>=0;i--) TABSET(this,p,i,STACKPULL(this));
	return push(PNTTOVAL(p));
}

const char* Memory::errorname(int err)
{
	if (err==MTLERR_OM) return "Out of memory";
	else if (err==MTLERR_OP) return "Unknown Operand";
	else if (err==MTLERR_DIV) return "Division by zero";
	else if (err==MTLERR_RET) return "Bad implementation on 'return'";
	else if (err==MTLERR_NOFUN) return "No function defined";
	else if (err==MTLERR_SN) return "Syntax error";
	else if (err==MTLERR_TYPE) return "Typechecking error";
	else if (err==MTLERR_ABORT) return "Application aborted";
	else return "Unknown error";
}
