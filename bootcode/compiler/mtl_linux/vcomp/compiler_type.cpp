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
#include "parser.h"
#include "prodbuffer.h"
#include "compiler.h"

// types

// création d'un noeud de type basique (éventuellement paramétrique)
int Compiler::createnodetypecore(const char* name)
{
	int k;

	int* p=MALLOCCLEAR(m,TYPEHEADER_LENGTH+2);
	if (!p) return MTLERR_OM;
	if (STACKPUSH(m,PNTTOVAL(p))) return MTLERR_OM;

	if (k=STRPUSH(m,name)) return k;

	TABSET(m,p,TYPEHEADER_CODE,INTTOVAL(TYPENAME_CORE));
	TABSET(m,p,TYPEHEADER_LENGTH+1,STACKPULL(m));
	return 0;
}

// création d'un noeud de type basique (éventuellement paramétrique)
int Compiler::createnodetypecore(int name)
{
	int* p=MALLOCCLEAR(m,TYPEHEADER_LENGTH+2);
	if (!p) return MTLERR_OM;
	if (STACKPUSH(m,PNTTOVAL(p))) return MTLERR_OM;

	TABSET(m,p,TYPEHEADER_CODE,INTTOVAL(TYPENAME_CORE));
	TABSET(m,p,TYPEHEADER_LENGTH+1,name);
	return 0;
}

// création d'un noeud de type non basique
int Compiler::createnodetype(int type)
{
	int size=0;
	if ((type==TYPENAME_UNDEF)||(type==TYPENAME_WEAK)) size=0;
	if ((type==TYPENAME_LIST)||(type==TYPENAME_TAB)||(type==TYPENAME_REC)) size=1;
	if (type==TYPENAME_FUN) size=2;

	int* p=MALLOCCLEAR(m,TYPEHEADER_LENGTH+size);
	if (!p) return MTLERR_OM;
	if (STACKPUSH(m,PNTTOVAL(p))) return MTLERR_OM;

	TABSET(m,p,TYPEHEADER_CODE,INTTOVAL(type));
	return 0;
}

// création d'un noeud de type tuple (n éléments empilés)
// empile le résultat
int Compiler::createnodetuple(int size)
{
	int* p=MALLOCCLEAR(m,TYPEHEADER_LENGTH+size);
	if (!p) return MTLERR_OM;
	TABSET(m,p,TYPEHEADER_CODE,INTTOVAL(TYPENAME_TUPLE));
	int i; for(i=size-1;i>=0;i--) TABSET(m,p,TYPEHEADER_LENGTH+i,STACKPULL(m));
	if (STACKPUSH(m,PNTTOVAL(p))) return MTLERR_OM;
	return 0;
}

// création d'un noeud de type tuple dans la compilation de valeurs (n éléments empilés au rangs 0, 2, 4, 2n-2)
// empile le résultat, sans dépiler les valeurs du tuple
int Compiler::createnodetupleval(int size)
{
	int* p=MALLOCCLEAR(m,TYPEHEADER_LENGTH+size);
	if (!p) return MTLERR_OM;
	TABSET(m,p,TYPEHEADER_CODE,INTTOVAL(TYPENAME_TUPLE));
	int i; for(i=size-1;i>=0;i--) TABSET(m,p,TYPEHEADER_LENGTH+i,STACKGET(m,2*(size-1-i)));
	if (STACKPUSH(m,PNTTOVAL(p))) return MTLERR_OM;
	return 0;
}

// trouve le type équivalent
int* Compiler::actualtype(int* p)
{
	int vp=TABGET(p,TYPEHEADER_ACTUAL);
	if (vp!=NIL) return actualtype(VALTOPNT(vp));
	return p;
}

// production d'un type
int Compiler::parsegraph(Parser* p,int env,int mono,int rec,int labels,int newvars,int* rnode)
{
	int k,n;
	
	if (!p->next(0))
	{
		PRINTF(m)(LOG_RUNTIME,"Compiler : uncomplete type reaches EOF\n");
		return MTLERR_SN;
	}

	int val;
	if (!searchlabel_byname(labels,p->token,&val,NULL)) return STACKPUSH(m,val);
	if ((p->token[0]=='w')&&(isdecimal(p->token+1)))
    {
		if (k=createnodetype(TYPENAME_WEAK)) return k;
		return addlabel(labels,p->token,STACKGET(m,0),NIL);
    }
	if (!strcmp(p->token,"_"))
    {
		return createnodetype(TYPENAME_WEAK);
    }
	if ((p->token[0]=='u')&&(isdecimal(p->token+1)))
    {
		if (mono)
		{
			PRINTF(m)(LOG_RUNTIME,"Compiler : polymorphism (%s) not accepted here\n",p->token);
			return MTLERR_SN;
		}
		if (!newvars)
		{
			PRINTF(m)(LOG_RUNTIME,"Compiler : cannot accept linked variable %s here\n",p->token);
			return MTLERR_SN;
		}
		if (k=createnodetype(TYPENAME_UNDEF)) return k;
		return addlabel(labels,p->token,STACKGET(m,0),NIL);
    }
	else if ((p->token[0]=='r')&&(isdecimal(p->token+1)))
    {
		int i=mtl_atoi(p->token+1);
		if ((i<0)||(i>=rec))
		{
			PRINTF(m)(LOG_RUNTIME,"Compiler : recursivity out of range %d [0 %d[\n",i,rec);
			return MTLERR_SN;
		}
		if (k=createnodetype(TYPENAME_REC)) return k;
		TABSET(m,VALTOPNT(STACKGET(m,0)),TYPEHEADER_LENGTH,INTTOVAL(i));
		*rnode=1;
		return 0;
    }
	else if (!strcmp(p->token,"tab"))
    {
		if (k=createnodetype(TYPENAME_TAB)) return k;
		if (k=parsegraph(p,env,mono,rec+1,labels,newvars,rnode)) return k;
		TABSET(m,VALTOPNT(STACKGET(m,1)),TYPEHEADER_LENGTH,STACKGET(m,0));
		STACKDROP(m);
		return 0;
    }
	else if (!strcmp(p->token,"list"))
    {
		if (k=createnodetype(TYPENAME_LIST)) return k;
		if (k=parsegraph(p,env,mono,rec+1,labels,newvars,rnode)) return k;
		TABSET(m,VALTOPNT(STACKGET(m,1)),TYPEHEADER_LENGTH,STACKGET(m,0));
		STACKDROP(m);
		return 0;
    }
	else if (!strcmp(p->token,"fun"))
    {
		if (k=createnodetype(TYPENAME_FUN)) return k;
		int nblab;
		if (newvars)
		{
			if (k=parsegraph(p,env,mono,rec+1,labels,1,rnode)) return k;
			nblab=0;
		}
		else
		{
			nblab=nblabels(labels);
			if (k=parsegraph(p,env,mono,rec+1,labels,1,rnode)) return k;
			nblab=nblabels(labels)-nblab;
		}
		TABSET(m,VALTOPNT(STACKGET(m,1)),TYPEHEADER_LENGTH,STACKGET(m,0));
		STACKDROP(m);
		if (k=parsegraph(p,env,mono,rec+1,labels,newvars,rnode)) return k;
		TABSET(m,VALTOPNT(STACKGET(m,1)),TYPEHEADER_LENGTH+1,STACKGET(m,0));
		STACKDROP(m);
		removenlabels(labels,nblab);
		return 0;
	}
	else if (!strcmp(p->token,"["))
    {
		n=0;
		while(1)
        {
			if (!p->next(0))
			{
				PRINTF(m)(LOG_RUNTIME,"Compiler : uncomplete type reaches EOF\n");
				return MTLERR_SN;
			}
			if (!strcmp(p->token,"]"))
			{
				return createnodetuple(n);
			}
			else
            {
				p->giveback();
				if (k=parsegraph(p,env,mono,rec+1,labels,newvars,rnode)) return k;
				n++;
            }
        }
    }
	else if (islabel(p->token))
    {
		int* t=searchtype(env,p->token);
		if (!t)
		{
			PRINTF(m)(LOG_RUNTIME,"Compiler : unknown type %s\n",p->token);
			return MTLERR_SN;
		}
		int* q=VALTOPNT(TABGET(t,REF_TYPE));
		int vargs=TABGET(q,TYPEHEADER_LENGTH);
		if (vargs==NIL) return STACKPUSH(m,TABGET(t,REF_TYPE));
		else
		{
			if (k=createnodetypecore(TABGET(q,TYPEHEADER_LENGTH+1))) return k;
			int n=TABLEN(VALTOPNT(vargs));
			int* t0=MALLOCCLEAR(m,TABLEN(VALTOPNT(vargs)));
			if (!t0) return MTLERR_OM;
			if (k=STACKPUSH(m,PNTTOVAL(t0))) return k;
			if (!p->next(0))
			{
				PRINTF(m)(LOG_RUNTIME,"Compiler : '(' expected (found EOF)\n");
				return MTLERR_SN;
			}
			if (strcmp(p->token,"("))
			{
				PRINTF(m)(LOG_RUNTIME,"Compiler : '(' expected (found '%s')\n",parser->token);
				return MTLERR_SN;
			}
			int i;for(i=0;i<n;i++)
			{
				if (k=parsegraph(p,env,mono,rec+1,labels,newvars,rnode)) return k;
				TABSET(m,t0,i,STACKPULL(m));
			}
			if (!p->next(0))
			{
				PRINTF(m)(LOG_RUNTIME,"Compiler : ')' expected (found EOF)\n");
				return MTLERR_SN;
			}
			if (strcmp(p->token,")"))
			{
				PRINTF(m)(LOG_RUNTIME,"Compiler : ')' expected (found '%s')\n",parser->token);
				return MTLERR_SN;
			}
			TABSET(m,VALTOPNT(STACKGET(m,1)),TYPEHEADER_LENGTH,STACKGET(m,0));
			STACKDROP(m);
		}
    }
	else
	{
		PRINTF(m)(LOG_RUNTIME,"Compiler : unknown token %s\n",p->token);
		return MTLERR_SN;
	}
	return 0;
}

// gestion des noeuds rec : on les lie avec le champ ACTUAL
int Compiler::parse_rnode(int *p)
{
	int k,i;

	int c=VALTOINT(TABGET(p,TYPEHEADER_CODE));
	if ((c==TYPENAME_WEAK)||(c==TYPENAME_UNDEF)) return 0;	// type faible ou non défini, pas de récursion
	if ((c==TYPENAME_CORE)&&(TABGET(p,TYPEHEADER_LENGTH)==NIL)) return 0;	// type de base, non paramétrique

	if (c==TYPENAME_REC)
	{
		TABSET(m,p,TYPEHEADER_ACTUAL,STACKGET(m,VALTOINT(TABGET(p,TYPEHEADER_LENGTH))));
		return 0;
	}
	if (k=STACKPUSH(m,PNTTOVAL(p))) return k;

	if (c==TYPENAME_CORE)
	{
		int* tup=VALTOPNT(TABGET(p,TYPEHEADER_LENGTH));
		for(i=0;i<TABLEN(tup);i++) if (k=parse_rnode(VALTOPNT(TABGET(tup,i)))) return k;
	}
	else if (c==TYPENAME_FUN)
	{
		if (k=parse_rnode(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)))) return k;
		if (k=parse_rnode(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH+1)))) return k;
	}
	else if (c==TYPENAME_LIST)
	{
		if (k=parse_rnode(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)))) return k;
	}
	else if (c==TYPENAME_TAB)
	{
		if (k=parse_rnode(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)))) return k;
	}
	else if (c==TYPENAME_TUPLE)
	{
		for(i=TYPEHEADER_LENGTH;i<TABLEN(p);i++) if (k=parse_rnode(VALTOPNT(TABGET(p,i)))) return k;
	}
	STACKDROP(m);
	return 0;
}

// création d'un graphe de type directement à partir d'un parser (utile pour les types écrits dans le code source)
int Compiler::creategraph(Parser* p,int env,int mono)
{
	int k,labels;
	int rnode=0;
	if (k=STACKPUSH(m,NIL)) return k;
	labels=STACKREF(m);
//k=parsegraph(p,env,mono,0,labels,0,&rnode);
	if (k=parsegraph(p,env,mono,0,labels,0,&rnode)) return k;
	STACKSET(m,1,STACKGET(m,0));
	STACKDROP(m);
	if (rnode) return parse_rnode(VALTOPNT(STACKGET(m,0)));
	return 0;
}

// création d'un graphe de type directement à partir d'un parser (utile pour les types écrits dans le code source)
// avec une liste de labels pré-existante
int Compiler::creategraph(Parser* p,int env,int mono,int labels)
{
	int k;
	int rnode=0;
	if (k=parsegraph(p,env,mono,0,labels,0,&rnode)) return k;
	if (rnode) return parse_rnode(VALTOPNT(STACKGET(m,0)));
	return 0;
}


// création d'un graphe de type à partir d'une chaîne
int Compiler::creategraph(const char* src,int env,int mono)
{
//	PRINTF(m)(LOG_DEVCORE,"Compiler : creategraph : %s\n",src);

	Parser* p=new Parser(m->term,src);
	int k=creategraph(p,env,mono);
	delete p;
	return k;
}




int Compiler::recechograph(Prodbuffer *output,int* p,int rec,int labels)
{
	int i,k;
	p=actualtype(p);
	if (k=STACKPUSH(m,PNTTOVAL(p))) return k;

	for(i=0;i<rec;i++) if (STACKGET(m,0)==STACKGET(m,i+1))
	{
		STACKDROP(m);
		output->printf("r%d",i);
		return 0;
	}
	int v=VALTOINT(TABGET(p,TYPEHEADER_CODE));
	if (v==TYPENAME_CORE)
	{
		output->printf("%s",STRSTART(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH+1))));
		int vargs=TABGET(p,TYPEHEADER_LENGTH);
		if (vargs!=NIL)
		{
			output->printf("(");
			int* tup=VALTOPNT(vargs);
			for(i=0;i<TABLEN(tup);i++)
			{
				if (i) output->printf(" ");
				recechograph(output,VALTOPNT(TABGET(tup,i)),rec+1,labels);
			}
			output->printf(")");
		}
	}
	else if (v==TYPENAME_FUN)
	{
		output->printf("fun ");
		recechograph(output,VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)),rec+1,labels);
		output->printf(" ");
		recechograph(output,VALTOPNT(TABGET(p,TYPEHEADER_LENGTH+1)),rec+1,labels);
	}
	else if (v==TYPENAME_LIST)
	{
		output->printf("list ");
		recechograph(output,VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)),rec+1,labels);
	}
	else if (v==TYPENAME_TAB)
	{
		output->printf("tab ");
		recechograph(output,VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)),rec+1,labels);
	}
	else if (v==TYPENAME_TUPLE)
	{
		output->printf("[");
		for(i=TYPEHEADER_LENGTH;i<TABLEN(p);i++)
		{
			if (i>TYPEHEADER_LENGTH) output->printf(" ");
			recechograph(output,VALTOPNT(TABGET(p,i)),rec+1,labels);
		}
		output->printf("]");
	}
	else if (v==TYPENAME_UNDEF)
	{
		char* name;
		if (!searchlabel_byval(labels,PNTTOVAL(p),&name))
			output->printf("%s",name);
		else
		{
			char buf[16];
			sprintf(buf,"u%d",nblabels(labels));
			addlabel(labels,buf,PNTTOVAL(p),NIL);
			output->printf("%s",buf);
		}
	}
	else if (v==TYPENAME_WEAK)
	{
		char* name;
		if (!searchlabel_byval(labels,PNTTOVAL(p),&name))
			output->printf("%s",name);
		else
		{
			char buf[16];
			sprintf(buf,"w%d",nblabels(labels));
			addlabel(labels,buf,PNTTOVAL(p),NIL);
			output->printf("%s",buf);
		}
	}
	STACKDROP(m);
	return 0;
}


int Compiler::echograph(Prodbuffer *output,int* p)
{
	int k,labels;
	if (k=STACKPUSH(m,NIL)) return k;
	labels=STACKREF(m);
	recechograph(output,p,0,labels);
	STACKDROP(m);
	return 0;
}


// copie de graphe
int Compiler::reccopytype(int *p)
{
	int k,i;

	p=actualtype(p);
	int vq=TABGET(p,TYPEHEADER_COPY);
	if (vq!=NIL) return STACKPUSH(m,vq);	// élément déjà copié
	int c=VALTOINT(TABGET(p,TYPEHEADER_CODE));
	if ((c==TYPENAME_CORE)&&(TABGET(p,TYPEHEADER_LENGTH)==NIL))
		return STACKPUSH(m,PNTTOVAL(p));	// type basique, ne pas copier

	if (c==TYPENAME_TUPLE)
	{
		for(i=TYPEHEADER_LENGTH;i<TABLEN(p);i++) if (k=STACKPUSH(m,NIL)) return k;
		if (k=createnodetuple(TABLEN(p)-TYPEHEADER_LENGTH)) return k;
		TABSET(m,p,TYPEHEADER_COPY,STACKGET(m,0));	// positionne le champ 'copy' de l'original
		int* q=VALTOPNT(STACKGET(m,0));
		for(i=TYPEHEADER_LENGTH;i<TABLEN(p);i++)
		{
			if (k=reccopytype(VALTOPNT(TABGET(p,i)))) return k;
			TABSET(m,q,i,STACKPULL(m));
		}
		return 0;
	}

	if (c==TYPENAME_WEAK) return STACKPUSH(m,PNTTOVAL(p));	// type faible, ne pas copier

	if (c==TYPENAME_CORE) k=createnodetypecore(TABGET(p,TYPEHEADER_LENGTH+1));
	else k=createnodetype(c);
	if (k) return k;	// copie le noeud
	TABSET(m,p,TYPEHEADER_COPY,STACKGET(m,0));	// positionne le champ 'copy' de l'original

	int* q=VALTOPNT(STACKGET(m,0));
	if (c==TYPENAME_FUN)
	{
		if (k=reccopytype(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)))) return k;
		TABSET(m,q,TYPEHEADER_LENGTH,STACKPULL(m));
		if (k=reccopytype(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH+1)))) return k;
		TABSET(m,q,TYPEHEADER_LENGTH+1,STACKPULL(m));
	}
	else if (c==TYPENAME_LIST)
	{
		if (k=reccopytype(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)))) return k;
		TABSET(m,q,TYPEHEADER_LENGTH,STACKPULL(m));
	}
	else if (c==TYPENAME_TAB)
	{
		if (k=reccopytype(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)))) return k;
		TABSET(m,q,TYPEHEADER_LENGTH,STACKPULL(m));
	}
	else if (c==TYPENAME_CORE)
	{
		int vargs=TABGET(p,TYPEHEADER_LENGTH);
		if (vargs==NIL) return 0;
		else
		{
			int* tup=VALTOPNT(vargs);
			for(i=0;i<TABLEN(tup);i++) if (k=reccopytype(VALTOPNT(TABGET(tup,i)))) return k;
			if (k=DEFTAB(m,TABLEN(tup))) return k;
			TABSET(m,q,TYPEHEADER_LENGTH,STACKPULL(m));
		}
	}
	else if (c==TYPENAME_UNDEF)
	{
	}
	return 0;
}

// remise à nil du champ 'copy' d'un graphe
int Compiler::recresetcopy(int *p)
{
	int k,i;

	p=actualtype(p);
	int vq=TABGET(p,TYPEHEADER_COPY);
	if (vq==NIL) return 0;	// élément déjà reseté

	TABSET(m,p,TYPEHEADER_COPY,NIL);	// reset le champ 'copy'

	int c=VALTOINT(TABGET(p,TYPEHEADER_CODE));
	if (c==TYPENAME_WEAK) return 0;	// type faible, pas de récursion
	else if (c==TYPENAME_CORE)
	{
		int vargs=TABGET(p,TYPEHEADER_LENGTH);
		if (vargs==NIL) return 0;
		else
		{
			int* tup=VALTOPNT(vargs);
			for(i=0;i<TABLEN(tup);i++) if (k=recresetcopy(VALTOPNT(TABGET(tup,i)))) return k;
		}
	}
	else if (c==TYPENAME_FUN)
	{
		if (k=recresetcopy(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)))) return k;
		if (k=recresetcopy(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH+1)))) return k;
	}
	else if (c==TYPENAME_LIST)
	{
		if (k=recresetcopy(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)))) return k;
	}
	else if (c==TYPENAME_TAB)
	{
		if (k=recresetcopy(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)))) return k;
	}
	else if (c==TYPENAME_TUPLE)
	{
		for(i=TYPEHEADER_LENGTH;i<TABLEN(p);i++) if (k=recresetcopy(VALTOPNT(TABGET(p,i)))) return k;
	}
	else if (c==TYPENAME_UNDEF)
	{
	}
	return 0;
}

int Compiler::copytype(int *p)
{
	int k;
	if (k=reccopytype(p)) return k;
	if (k=recresetcopy(p)) return k;
	return 0;
}


// passage des UNDEF en WEAK
int Compiler::recgoweak(int *p)
{
	int k,i;

	p=actualtype(p);
	int vq=TABGET(p,TYPEHEADER_COPY);
	if (vq!=NIL) return 0;	// élément déjà traité
	int c=VALTOINT(TABGET(p,TYPEHEADER_CODE));
	if (c==TYPENAME_WEAK) return 0;	// type faible, ne pas traiter

	TABSET(m,p,TYPEHEADER_COPY,INTTOVAL(1));	// positionne le champ 'copy' de l'original
	if (c==TYPENAME_CORE)
	{
		int vargs=TABGET(p,TYPEHEADER_LENGTH);
		if (vargs==NIL) return 0;
		else
		{
			int* tup=VALTOPNT(vargs);
			for(i=0;i<TABLEN(tup);i++) if (k=recgoweak(VALTOPNT(TABGET(tup,i)))) return k;
		}
	}
	else if (c==TYPENAME_FUN)
	{
		if (k=recgoweak(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)))) return k;
		if (k=recgoweak(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH+1)))) return k;
	}
	else if (c==TYPENAME_LIST)
	{
		if (k=recgoweak(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)))) return k;
	}
	else if (c==TYPENAME_TAB)
	{
		if (k=recgoweak(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH)))) return k;
	}
	else if (c==TYPENAME_TUPLE)
	{
		for(i=TYPEHEADER_LENGTH;i<TABLEN(p);i++) if (k=recgoweak(VALTOPNT(TABGET(p,i)))) return k;
	}
	else if (c==TYPENAME_UNDEF)
	{
		TABSET(m,p,TYPEHEADER_CODE,INTTOVAL(TYPENAME_WEAK));
	}
	return 0;
}


// unification de graphe
int Compiler::restoreactual(int* t,int* s,int vt,int vs,int k)
{
	TABSET(m,t,TYPEHEADER_ACTUAL,vt);
	TABSET(m,s,TYPEHEADER_ACTUAL,vs);
	return k;
}

int Compiler::recunif(int* s,int* t)
{
	s=actualtype(s);
	t=actualtype(t);
	if (s==t) return 0;	// ceci gère le cas des types basiques
	
	int ns=VALTOINT(TABGET(s,TYPEHEADER_CODE));
	int nt=VALTOINT(TABGET(t,TYPEHEADER_CODE));
	int ok=1;
	if ((ns!=TYPENAME_UNDEF)&&(ns!=TYPENAME_WEAK)
		&&(nt!=TYPENAME_UNDEF)&&(nt!=TYPENAME_WEAK) // si les deux noeuds bien définis ...
		&&(ns!=nt) ) ok=0;
	else if ((ns==nt)&&(ns==TYPENAME_CORE)&&(TABGET(s,TYPEHEADER_LENGTH+1)!=TABGET(t,TYPEHEADER_LENGTH+1))) ok=0;
	if (!ok)
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : ");
		echonode(ns,s);
		PRINTF(m)(LOG_COMPILER," cannot be unified with ");
		echonode(nt,t);
		PRINTF(m)(LOG_COMPILER,"\n");
		return MTLERR_TYPE; // ... il doivent être du même type
	}
	
	int vt=TABGET(t,TYPEHEADER_ACTUAL);	// on sauvegarde avant unification
	int vs=TABGET(s,TYPEHEADER_ACTUAL);

	if ((ns==TYPENAME_UNDEF) /* union des noeuds */
		|| ((ns==TYPENAME_WEAK)&&(nt!=TYPENAME_UNDEF)) )
	{
		TABSET(m,s,TYPEHEADER_ACTUAL,PNTTOVAL(t));
		if (ns==TYPENAME_WEAK)
		{
			recgoweak(t);
			recresetcopy(t);
		}
	}
	else
	{
		TABSET(m,t,TYPEHEADER_ACTUAL,PNTTOVAL(s));
		if (nt==TYPENAME_WEAK)
		{
			recgoweak(s);
			recresetcopy(s);
		}
	}

	if (ns==nt)
	{
		int k;
		if ((ns==TYPENAME_TAB)||(ns==TYPENAME_LIST)||(ns==TYPENAME_FUN))
		{
			if (k=recunif(VALTOPNT(TABGET(s,TYPEHEADER_LENGTH)),VALTOPNT(TABGET(t,TYPEHEADER_LENGTH))))
				return restoreactual(t,s,vt,vs,k);
			if ((ns==TYPENAME_FUN)
				&&(k=recunif(VALTOPNT(TABGET(s,TYPEHEADER_LENGTH+1)),VALTOPNT(TABGET(t,TYPEHEADER_LENGTH+1)))) )
					return restoreactual(t,s,vt,vs,k);
		}
		else if (ns==TYPENAME_TUPLE)
		{
			int len=TABLEN(s);
			if (len!=TABLEN(t)) return restoreactual(t,s,vt,vs,MTLERR_TYPE);
			int i; for(i=TYPEHEADER_LENGTH;i<len;i++)
				if (k=recunif(VALTOPNT(TABGET(s,i)),VALTOPNT(TABGET(t,i))))
					return restoreactual(t,s,vt,vs,k);
		}
		else if (ns==TYPENAME_CORE)
		{
			int vtups=TABGET(s,TYPEHEADER_LENGTH);
			int vtupt=TABGET(t,TYPEHEADER_LENGTH);
			if ((vtups==NIL)&&(vtupt==NIL)) return 0;
			if ((vtups==NIL)||(vtupt==NIL)) return restoreactual(t,s,vt,vs,MTLERR_TYPE);
			int* tups=VALTOPNT(vtups);
			int* tupt=VALTOPNT(vtupt);
			int len=TABLEN(tups);
			if (len!=TABLEN(tupt)) return restoreactual(t,s,vt,vs,MTLERR_TYPE);
			int i; for(i=0;i<len;i++)
				if (k=recunif(VALTOPNT(TABGET(tups,i)),VALTOPNT(TABGET(tupt,i))))
					return restoreactual(t,s,vt,vs,k);
		}
	}
	return 0;
}

int Compiler::unif(int* x,int* y)
{
	int l;

	if (!(l=recunif(x,y))) return 0;

	Prodbuffer* output=new Prodbuffer();

	output->printf("Compiler :\n ");
	echograph(output,x);
	output->printf("\ndoes not match with\n ");
	echograph(output,y);
	output->printf("\n");

	PRINTF(m)(LOG_COMPILER,"%s",output->getstart());
	delete output;

	return l;
}

// [fun [arg]]
int Compiler::unif_argfun()
{
	int k;
	int* fun=VALTOPNT(STACKPULL(m));
	int* arg=VALTOPNT(STACKGET(m,0));
	
	if (k=unif(VALTOPNT(TABGET(fun,TYPEHEADER_LENGTH)),arg)) return k;
	STACKSET(m,0,TABGET(fun,TYPEHEADER_LENGTH+1));
	return 0;
}

int* Compiler::argsfromfun(int *f)
{
	return VALTOPNT(TABGET(f,TYPEHEADER_LENGTH));
}

void Compiler::echonode(int code,int* p)
{
	if (code==TYPENAME_CORE) PRINTF(m)(LOG_COMPILER,"%s",STRSTART(VALTOPNT(TABGET(p,TYPEHEADER_LENGTH+1))));
	else if (code==TYPENAME_UNDEF) PRINTF(m)(LOG_COMPILER,"u*");
	else if (code==TYPENAME_WEAK) PRINTF(m)(LOG_COMPILER,"w*");
	else if (code==TYPENAME_TAB) PRINTF(m)(LOG_COMPILER,"tab");
	else if (code==TYPENAME_LIST) PRINTF(m)(LOG_COMPILER,"list");
	else if (code==TYPENAME_TUPLE) PRINTF(m)(LOG_COMPILER,"tuple");
	else if (code==TYPENAME_FUN) PRINTF(m)(LOG_COMPILER,"fun");
}
		
// unification d'un plus grand vers un plus petit
// attention, OPsearch suppose que seul MTLERR_TYPE peut arriver
int Compiler::recunifbigger(int* s,int* t)
{
	int* s0=s;	// on retient le s initial
	s=actualtype(s);
	t=actualtype(t);
	if (s==t) return 0;	// ceci gère le cas des types basiques
	
	int ns=VALTOINT(TABGET(s,TYPEHEADER_CODE));
	int nt=VALTOINT(TABGET(t,TYPEHEADER_CODE));
	int ok=1;
	if ((ns!=TYPENAME_UNDEF)&&(ns!=TYPENAME_WEAK)
		&&(nt!=TYPENAME_UNDEF)&&(nt!=TYPENAME_WEAK) // si les deux noeuds bien définis ...
		&&(ns!=nt) ) ok=0;
	else if ((ns==nt)&&(ns==TYPENAME_CORE)&&(TABGET(s,TYPEHEADER_LENGTH+1)!=TABGET(t,TYPEHEADER_LENGTH+1))) ok=0;
	if (!ok)
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : ");
		echonode(ns,s);
		PRINTF(m)(LOG_COMPILER," cannot be unified with ");
		echonode(nt,t);
		PRINTF(m)(LOG_COMPILER,"\n");
		return MTLERR_TYPE; // ... il doivent être du même type
	}
	
	int vt=TABGET(t,TYPEHEADER_ACTUAL);	// on sauvegarde avant unification
	int vs=TABGET(s,TYPEHEADER_ACTUAL);

	if ((nt==TYPENAME_UNDEF)&&(ns!=TYPENAME_UNDEF))
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : ");
		echonode(ns,s);
		PRINTF(m)(LOG_COMPILER," is smaller than ");
		echonode(nt,t);
		PRINTF(m)(LOG_COMPILER,"\n");
		return MTLERR_TYPE;
	}
	if ((ns==TYPENAME_UNDEF)&&(s0!=s))
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : u* already unified\n");
		return MTLERR_TYPE;
	}

	if ((ns==TYPENAME_UNDEF) /* union des noeuds */
		|| ((ns==TYPENAME_WEAK)&&(nt!=TYPENAME_UNDEF)) )
	{
		TABSET(m,s,TYPEHEADER_ACTUAL,PNTTOVAL(t));
		if (ns==TYPENAME_WEAK)
		{
			recgoweak(t);
			recresetcopy(t);
		}
	}
	else
	{
		TABSET(m,t,TYPEHEADER_ACTUAL,PNTTOVAL(s));
		if (nt==TYPENAME_WEAK)
		{
			recgoweak(s);
			recresetcopy(s);
		}
	}

	if (ns==nt)
	{
		int k;
		if ((ns==TYPENAME_TAB)||(ns==TYPENAME_LIST)||(ns==TYPENAME_FUN))
		{
			if (k=recunifbigger(VALTOPNT(TABGET(s,TYPEHEADER_LENGTH)),VALTOPNT(TABGET(t,TYPEHEADER_LENGTH))))
				return restoreactual(t,s,vt,vs,k);
			if ((ns==TYPENAME_FUN)
				&&(k=recunifbigger(VALTOPNT(TABGET(s,TYPEHEADER_LENGTH+1)),VALTOPNT(TABGET(t,TYPEHEADER_LENGTH+1)))) )
					return restoreactual(t,s,vt,vs,k);
		}
		else if (ns==TYPENAME_TUPLE)
		{
			int len=TABLEN(s);
			if (len!=TABLEN(t)) return restoreactual(t,s,vt,vs,MTLERR_TYPE);
			int i; for(i=TYPEHEADER_LENGTH;i<len;i++)
				if (k=recunifbigger(VALTOPNT(TABGET(s,i)),VALTOPNT(TABGET(t,i))))
					return restoreactual(t,s,vt,vs,k);
		}
		else if (ns==TYPENAME_CORE)
		{
			int vtups=TABGET(s,TYPEHEADER_LENGTH);
			int vtupt=TABGET(t,TYPEHEADER_LENGTH);
			if ((vtups==NIL)&&(vtupt==NIL)) return 0;
			if ((vtups==NIL)||(vtupt==NIL)) restoreactual(t,s,vt,vs,MTLERR_TYPE);
			int* tups=VALTOPNT(vtups);
			int* tupt=VALTOPNT(vtupt);
			int len=TABLEN(tups);
			if (len!=TABLEN(tupt)) return restoreactual(t,s,vt,vs,MTLERR_TYPE);
			int i; for(i=0;i<len;i++)
				if (k=recunifbigger(VALTOPNT(TABGET(tups,i)),VALTOPNT(TABGET(tupt,i))))
					return restoreactual(t,s,vt,vs,k);
		}
	}
	return 0;
}

int Compiler::unifbigger(int* x,int* y)
{
	int l;

	if (!(l=recunifbigger(x,y))) return 0;

	Prodbuffer* output=new Prodbuffer();

	output->printf("Compiler :\n ");
	echograph(output,x);
	output->printf("\ndoes not match with or is smaller than\n ");
	echograph(output,y);
	output->printf("\n");

	PRINTF(m)(LOG_COMPILER,"%s",output->getstart());
	delete output;

	return l;
}
