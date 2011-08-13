//-------------------
// MV
// version WIN32 et POCKETPC
// Sylvain Huet
// Derniere mise a jour : 07/01/2003
//

#include <stdio.h>
#include <string.h>

#include "param.h"
#include "terminal.h"
#include "memory.h"
#include "prodbuffer.h"
#include "interpreter.h"
#include "compiler.h"


Compiler::Compiler(Memory* mem)
{
	m=mem;
	bc=new Prodbuffer();
	outputbuf=new Prodbuffer();
	brelease=new Prodbuffer();
};

Compiler::~Compiler()
{
	delete bc;
	delete outputbuf;
	delete brelease;
};


int Compiler::start()
{
	int k;

	// création du package systeme
	if (k=createpackage("system",8)) return k;
	systempackage=STACKGET(m,0);
	m->addroot(VALTOPNT(systempackage));	// le package systeme devient une racine
	if (k=addstdlibcore()) return k;
	int pck=STACKREF(m);

	// création du tableau des types utilisés par le compilateur
	if (k=creategraph("I",systempackage,0)) return k;
	if (k=creategraph("F",systempackage,0)) return k;
	if (k=creategraph("S",systempackage,0)) return k;
	if (k=creategraph("Env",systempackage,0)) return k;
	if (k=creategraph("Xml",systempackage,0)) return k;

	if (k=creategraph("fun[u0 list u0]list u0",systempackage,0)) return k;
	if (k=creategraph("fun[tab u0 I]u0",systempackage,0)) return k;
	if (k=creategraph("fun[fun u0 u1 u0]u1",systempackage,0)) return k;
	if (k=DEFTAB(m,STDTYPE_LENGTH)) return k;
	stdtypes=VALTOPNT(STACKPULL(m));
	m->addroot(stdtypes);

//	dumppackage(systempackage);
	return 0;
}

void Compiler::stop()
{
}

int Compiler::getsystempackage()
{
	return systempackage;
}

// crée et empile un package
int Compiler::createpackage(const char* name,int loghach)
{
	int k;
	if (k=PUSHMALLOCCLEAR(m,PACK_LENGTH)) return k;
	int* p=VALTOPNT(STACKGET(m,0));

	if (k=STRPUSH(m,name)) return k;
	TABSET(m,p,PACK_NAME,STACKPULL(m));

	if (k=PUSHMALLOCCLEAR(m,1+(1<<loghach))) return k;
	TABSET(m,p,PACK_HACH,STACKPULL(m));

	return 0;
}

// calcule la fonction de hachage d'une chaine (entre 0 et 255)
int Compiler::hash(char* name)
{
	int v=0;
	int i=0;
	while(*name)
	{
		int c=(*name++)&255;
		c=c<<i;
		v+=c+(c>>8);
		i=(i+1)&7;
	}
	return v&255;
}

// ajoute une référence à un package
// [ref package] -> [package]
void Compiler::addreftopackage(int* ref,int* package)
{
	int v;
	if (TABGET(ref,REF_NAME)!=NIL) v=hash(STRSTART(VALTOPNT(TABGET(ref,REF_NAME))));
	else v=hash(STRSTART(VALTOPNT(TABGET(VALTOPNT(TABGET(ref,REF_TYPE)),TYPEHEADER_LENGTH+1))));

	int* p=VALTOPNT(TABGET(package,PACK_HACH));

//	TABSET(m,ref,REF_PACKAGE,PNTTOVAL(package));
	v&=TABLEN(p)-2;
	TABSET(m,ref,REF_NEXT,TABGET(p,v));
	TABSET(m,p,v,PNTTOVAL(ref));
	v=TABLEN(p)-1;
	TABSET(m,ref,REF_NEXTALL,TABGET(p,v));
	TABSET(m,p,v,PNTTOVAL(ref));
}

// recherche d'un type dans un environnement
int* Compiler::searchtype(int env,char* name)
{
	int v=hash(name);
	while(env!=NIL)
	{
		int* p=VALTOPNT(TABGET(VALTOPNT(env),PACK_HACH));
		int vref=TABGET(p,v&(TABLEN(p)-2));
		while(vref!=NIL)
		{
			int* ref=VALTOPNT(vref);
			if ((TABGET(ref,REF_NAME)==NIL)
				&&(!strcmp(name,STRSTART(VALTOPNT(TABGET(VALTOPNT(TABGET(ref,REF_TYPE)),TYPEHEADER_LENGTH+1))))) )
				return ref;
			vref=TABGET(ref,REF_NEXT);
		}
		env=TABGET(VALTOPNT(env),PACK_NEXT);
	}
	return NULL;
}

// recherche d'un type non défini dans un environnement
int* Compiler::searchemptytype(int env,char* name)
{
	int v=hash(name);
	while(env!=NIL)
	{
		int* p=VALTOPNT(TABGET(VALTOPNT(env),PACK_HACH));
		int vref=TABGET(p,v&(TABLEN(p)-2));
		while(vref!=NIL)
		{
			int* ref=VALTOPNT(vref);
			if ((TABGET(ref,REF_CODE)==INTTOVAL(CODE_EMPTYTYPE))
				&&(!strcmp(name,STRSTART(VALTOPNT(TABGET(VALTOPNT(TABGET(ref,REF_TYPE)),TYPEHEADER_LENGTH+1))))) )
				return ref;
			vref=TABGET(ref,REF_NEXT);
		}
		env=TABGET(VALTOPNT(env),PACK_NEXT);
	}
	return NULL;
}

void Compiler::dumppackage(int env)
{
	while(env!=NIL)
	{
		PRINTF(m)(LOG_DEVCORE,"package %s",STRSTART(VALTOPNT(TABGET(VALTOPNT(env),PACK_NAME))));
		int* p=VALTOPNT(TABGET(VALTOPNT(env),PACK_HACH));
		int vref=TABGET(p,TABLEN(p)-1);
		while(vref!=NIL)
		{
			int* ref=VALTOPNT(vref);
			char* name=NULL;
			if (TABGET(ref,REF_NAME)!=NIL) name=STRSTART(VALTOPNT(TABGET(ref,REF_NAME)));
			if (!name) name=STRSTART(VALTOPNT(TABGET(VALTOPNT(TABGET(ref,REF_TYPE)),TYPEHEADER_LENGTH+1)));
			if (name) PRINTF(m)(LOG_DEVCORE," %s",name);
			vref=TABGET(ref,REF_NEXTALL);
		}
		PRINTF(m)(LOG_DEVCORE,"\n");
		env=TABGET(VALTOPNT(env),PACK_NEXT);
	}
}

int Compiler::searchbytype(int env,int type)
{
	while(env!=NIL)
	{
		int* p=VALTOPNT(TABGET(VALTOPNT(env),PACK_HACH));
		int vref=TABGET(p,TABLEN(p)-1);
		while(vref!=NIL)
		{
			if (TABGET(VALTOPNT(vref),REF_TYPE)==type) return vref;
			vref=TABGET(VALTOPNT(vref),REF_NEXTALL);
		}
		env=TABGET(VALTOPNT(env),PACK_NEXT);
	}
	return NIL;
}

int Compiler::fillproto(int env,int* fun)
{
	int k;
	int v=hash(STRSTART(VALTOPNT(TABGET(fun,REF_NAME))));
	while(env!=NIL)
	{
		int* p=VALTOPNT(TABGET(VALTOPNT(env),PACK_HACH));
		int vref=TABGET(p,v&(TABLEN(p)-2));
		while(vref!=NIL)
		{
			int* ref=VALTOPNT(vref);
			if ((ref!=fun)&&(VALTOINT(TABGET(ref,REF_CODE))>=0))	// on recherche des fonctions
			{
				if (  (!strcmp(STRSTART(VALTOPNT(TABGET(fun,REF_NAME))),STRSTART(VALTOPNT(TABGET(ref,REF_NAME)))))
					&&(TABGET(ref,REF_VAL)==NIL) )
				{
					if (k=unifbigger(VALTOPNT(TABGET(fun,REF_TYPE)),VALTOPNT(TABGET(ref,REF_TYPE))))
					{
						PRINTF(m)(LOG_COMPILER,"Compiler : prototype does not match\n");
						return k;
					}
					TABSET(m,ref,REF_VAL,TABGET(fun,REF_VAL));
//					TABSET(m,ref,REF_PACKAGE,TABGET(fun,REF_PACKAGE));
				}
			}
			vref=TABGET(ref,REF_NEXT);
		}
		env=TABGET(VALTOPNT(env),PACK_NEXT);
	}
	return 0;
}

int Compiler::findproto(int env,int* fun)
{
	int v=hash(STRSTART(VALTOPNT(TABGET(fun,REF_NAME))));
	while(env!=NIL)
	{
		int* p=VALTOPNT(TABGET(VALTOPNT(env),PACK_HACH));
		int vref=TABGET(p,v&(TABLEN(p)-2));
		while(vref!=NIL)
		{
			int* ref=VALTOPNT(vref);
			if ((ref!=fun)&&(VALTOINT(TABGET(ref,REF_CODE))>=0))	// on recherche des fonctions
			{
				if (  (!strcmp(STRSTART(VALTOPNT(TABGET(fun,REF_NAME))),STRSTART(VALTOPNT(TABGET(ref,REF_NAME)))))
					&&(TABGET(ref,REF_VAL)==NIL) )
				{
					return TABGET(ref,REF_PACKAGE);
				}
			}
			vref=TABGET(ref,REF_NEXT);
		}
		env=TABGET(VALTOPNT(env),PACK_NEXT);
	}
	return NIL;
}

// recherche d'une référence dans un environnement
int* Compiler::searchref(int env,char* name)
{
	int v=hash(name);
	while(env!=NIL)
	{
		int* p=VALTOPNT(TABGET(VALTOPNT(env),PACK_HACH));
		int vref=TABGET(p,v&(TABLEN(p)-2));
		while(vref!=NIL)
		{
			int* ref=VALTOPNT(vref);
			if ((TABGET(ref,REF_NAME)!=NIL)
				&&(!strcmp(name,STRSTART(VALTOPNT(TABGET(ref,REF_NAME))))) )
				{
					// à partir du moment où on recherche cette référence, c'est
					// qu'elle est utlisé quelque part, donc...
					TABSET(m,ref,REF_USED,INTTOVAL(1));

					return ref;
				}
			vref=TABGET(ref,REF_NEXT);
		}
		env=TABGET(VALTOPNT(env),PACK_NEXT);
	}
	return NULL;
}

// recherche d'une référence dans un environnement, ne marque pas la référence comme utilisée
int* Compiler::searchref_nosetused(int env,char* name)
{
	int v=hash(name);
	while(env!=NIL)
	{
		int* p=VALTOPNT(TABGET(VALTOPNT(env),PACK_HACH));
		int vref=TABGET(p,v&(TABLEN(p)-2));
		while(vref!=NIL)
		{
			int* ref=VALTOPNT(vref);
			if ((TABGET(ref,REF_NAME)!=NIL)
				&&(!strcmp(name,STRSTART(VALTOPNT(TABGET(ref,REF_NAME))))) )
				return ref;
			vref=TABGET(ref,REF_NEXT);
		}
		env=TABGET(VALTOPNT(env),PACK_NEXT);
	}
	return NULL;
}



// ajout d'un label dans une liste de labels
int Compiler::addlabel(int base,const char* name,int val,int ref)
{
	int k;
	int* p=MALLOCCLEAR(m,LABELLIST_LENGTH);
	if (!p) return MTLERR_OM;
	TABSET(m,p,LABELLIST_NEXT,STACKGETFROMREF(m,base,0));
	STACKSETFROMREF(m,base,0,PNTTOVAL(p));

	if (k=STRPUSH(m,name)) return k;
	TABSET(m,p,LABELLIST_NAME,STACKPULL(m));
	TABSET(m,p,LABELLIST_VAL,val);
	TABSET(m,p,LABELLIST_REF,ref);
	return 0;
}

// compte le nombre de labels
int Compiler::nblabels(int base)
{
	int n=0;
	int vlab=STACKGETFROMREF(m,base,0);
	while(vlab!=NIL)
	{
		n++;
		vlab=TABGET(VALTOPNT(vlab),LABELLIST_NEXT);
	}
	return n;
}

void Compiler::removenlabels(int base,int n)
{
	while(n>0)
	{
		STACKSETFROMREF(m,base,0,TABGET(VALTOPNT(STACKGETFROMREF(m,base,0)),LABELLIST_NEXT));
		n--;
	}
}

// recherche d'un label dans une liste de labels
int Compiler::searchlabel_byname(int base,char* name,int* val,int* ref)
{
	int vlab=STACKGETFROMREF(m,base,0);
	while(vlab!=NIL)
	{
		int* lab=VALTOPNT(vlab);
		if (!strcmp(name,STRSTART(VALTOPNT(TABGET(lab,LABELLIST_NAME)))) )
		{
			if (val) *val=TABGET(lab,LABELLIST_VAL);
			if (ref) *ref=TABGET(lab,LABELLIST_REF);
			return 0;
		}
		vlab=TABGET(lab,LABELLIST_NEXT);
	}
	return -1;
}

// recherche d'un label dans une liste de labels
int Compiler::searchlabel_byval(int base,int val,char** name)
{
	int vlab=STACKGETFROMREF(m,base,0);
	while(vlab!=NIL)
	{
		int* lab=VALTOPNT(vlab);
		if (val==TABGET(lab,LABELLIST_VAL))
		{
			*name=STRSTART(VALTOPNT(TABGET(lab,LABELLIST_NAME)));
			return 0;
		}
		vlab=TABGET(lab,LABELLIST_NEXT);
	}
	return -1;
}

// création d'un tuple à partir d'une liste de labels
int* Compiler::tuplefromlabels(int base)
{
	int n=nblabels(base);
	int* t=MALLOC(m,n,TYPE_TAB);
	if (!t) return t;

	int vlab=STACKGETFROMREF(m,base,0);
	int i; for(i=n-1;i>=0;i--)
	{
		int* lab=VALTOPNT(vlab);
		TABSET(m,t,i,TABGET(lab,LABELLIST_REF));
		vlab=TABGET(lab,LABELLIST_NEXT);
	}
	return t;
}


// ajoute des fonctions à un package
int Compiler::addnative(int nref, const char** nameref, int* valref
              , int* coderef, const char** typeref,void* arg)
{
	int i,k;
	
	for(i=0;i<nref;i++)
    {
		int* p=MALLOCCLEAR(m,REF_LENGTH);
		if (!p) return MTLERR_OM;
		if (k=STACKPUSH(m,PNTTOVAL(p))) return MTLERR_OM;

		if (nameref[i])
		{
			if (k=STRPUSH(m,nameref[i])) return k;
			TABSET(m,p,REF_NAME,STACKPULL(m));
		}

		TABSET(m,p,REF_CODE,INTTOVAL(coderef[i]));

		if (coderef[i]>=0)
		{
			if (k=creategraph(typeref[i],systempackage,0)) return k;
			TABSET(m,p,REF_TYPE,STACKPULL(m));

//			if (k=PUSHPNT(m,(int*)valref[i])) return k;
			if (k=STACKPUSH(m,INTTOVAL(valref[i]))) return k;

			int* fun=MALLOCCLEAR(m,FUN_LENGTH);
			if (!fun) return MTLERR_OM;
			TABSET(m,p,REF_VAL,PNTTOVAL(fun));
	
			TABSET(m,fun,FUN_NBARGS,INTTOVAL(coderef[i]));
			TABSET(m,fun,FUN_NBLOCALS,NIL);
//			PRINTF(m)(1,"valref %s = %x\n",nameref[i],valref[i]);
//			TABSET(m,fun,FUN_BC,((int)valref[i])<<1);
			TABSET(m,fun,FUN_BC,STACKPULL(m));
			TABSET(m,fun,FUN_REF,(int)arg);
			TABSET(m,fun,FUN_REFERENCE,PNTTOVAL(p));
		}
		else if ((coderef[i]==CODE_VAR)
			||(coderef[i]==CODE_CONS)||(coderef[i]==CODE_CONS0))
		{
			if (k=creategraph(typeref[i],systempackage,0)) return k;
			TABSET(m,p,REF_TYPE,STACKPULL(m));
			TABSET(m,p,REF_VAL,(int)valref[i]);
		}
		else if (coderef[i]==CODE_FIELD)
		{
			if (k=creategraph(typeref[i],systempackage,0)) return k;
			if (k=STACKPUSH(m,(int)valref[i])) return k;
			int vtype=searchbytype(systempackage,
				TABGET(argsfromfun(VALTOPNT(STACKGET(m,1))),TYPEHEADER_LENGTH));
			if (k=STACKPUSH(m,vtype)) return k;
			if (k=DEFTAB(m,2)) return k;
			TABSET(m,p,REF_VAL,STACKPULL(m));
			TABSET(m,p,REF_TYPE,STACKPULL(m));
		}
		else if ((coderef[i]==CODE_TYPE)||(coderef[i]==CODE_SUM)||(coderef[i]==CODE_STRUCT))
		{
			if (k=createnodetypecore(typeref[i])) return k;
			TABSET(m,p,REF_TYPE,STACKPULL(m));
			TABSET(m,p,REF_VAL,(int)valref[i]);
		}
		addreftopackage(p,VALTOPNT(systempackage));
		STACKDROP(m);
	}
	return 0;
}


