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
#include "interpreter.h"

// switch 8 / 32 bits
void Compiler::bc_byte_or_int(int val,int opbyte,int opint)
{
		if ((val>=0)&&(val<=255))
		{
			bc->addchar(opbyte);
			bc->addchar(val);
		}
		else
		{
			bc->addchar(OPint);
			bc->addint(val);
			bc->addchar(opint);
		}
}

void Compiler::bcint_byte_or_int(int val)
{
		if ((val>=0)&&(val<=255))
		{
			bc->addchar(OPintb);
			bc->addchar(val);
		}
		else
		{
			bc->addchar(OPint);
			bc->addint(val);
		}
}


int Compiler::parseterm()
{
	int k;

	if (!parser->next(0))
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : term expected (found EOF)\n");
		return MTLERR_SN;
	}
	if (!strcmp(parser->token,"("))	// gestion des parenthèses
	{
		if (k=parseprogram()) return k;
		return parser->parsekeyword(")");
	}
	else if (!strcmp(parser->token,"["))	// gestion des tuples
	{
		if (parser->next(0))
		{
			if (islabel(parser->token))
			{
				int *p;
				if ((p=searchref(PNTTOVAL(newpackage),parser->token))	// recherche dans les autres globales
					&&(VALTOINT(TABGET(p,REF_CODE))==CODE_FIELD))
				return parsefields(p);
			}
			parser->giveback();
		}
		int nval=0;
		while(1)
		{
			if (!parser->next(0))
			{
				PRINTF(m)(LOG_COMPILER,"Compiler : ']' expected (found EOF)\n");
				return MTLERR_SN;
			}
			if (!strcmp(parser->token,"]"))
			{
				bc_byte_or_int(nval,OPdeftabb,OPdeftab);
				return createnodetuple(nval);
			}
			parser->giveback();
			if (k=parseexpression()) return k;
			nval++;
		}
	}
	else if (!strcmp(parser->token,"{"))	// gestion des tableaux
	{
		int nval=0;
		if (k=createnodetype(TYPENAME_TAB)) return k;
		if (k=createnodetype(TYPENAME_UNDEF)) return k;
		TABSET(m,VALTOPNT(STACKGET(m,1)),TYPEHEADER_LENGTH,STACKGET(m,0));
		int* p=VALTOPNT(STACKPULL(m));

		while(1)
		{
			if (!parser->next(0))
			{
				PRINTF(m)(LOG_COMPILER,"Compiler : '}' expected (found EOF)\n");
				return MTLERR_SN;
			}
			if (!strcmp(parser->token,"}"))
			{
				bc_byte_or_int(nval,OPdeftabb,OPdeftab);
				return 0;
			}
			parser->giveback();
			if (k=parseexpression()) return k;
			if (k=unif(VALTOPNT(STACKGET(m,0)),p)) return k;
			STACKDROP(m);
			nval++;
		}
	}
	else if (!strcmp(parser->token,"if"))	// gestion du if
		return parseif();
	else if (!strcmp(parser->token,"let"))	// gestion du let
		return parselet();
	else if (!strcmp(parser->token,"set"))	// gestion du set
		return parseset();
	else if (!strcmp(parser->token,"while"))	// gestion du while
		return parsewhile();
	else if (!strcmp(parser->token,"for"))	// gestion du for
		return parsefor();
	else if (!strcmp(parser->token,"match"))	// gestion du match
		return parsematch();
	else if (!strcmp(parser->token,"call"))	// gestion du while
		return parsecall();
	else if (!strcmp(parser->token,"update"))	// gestion du update
		return parseupdate();
	else if (!strcmp(parser->token,"nil"))	// gestion du nil
	{
		bc->addchar(OPnil);
		return createnodetype(TYPENAME_UNDEF);
	}
	else if (!strcmp(parser->token,"'"))	// gestion des 'char
	{
		if (!parser->next(0))
		{
			PRINTF(m)(LOG_COMPILER,"Compiler : 'char expected (found EOF)\n");
			return MTLERR_SN;
		}
		bcint_byte_or_int(parser->token[0]&255);
		if (k=parser->parsekeyword("'")) return k;
		return STACKPUSH(m,TABGET(stdtypes,STDTYPE_I));
	}
	else if (islabel(parser->token))	// gestion des appels de fonctions ou références
		return parseref();
	else if (isdecimal(parser->token))	// gestion des entiers
	{
		int i=mtl_atoi(parser->token);
		bcint_byte_or_int(i);
		return STACKPUSH(m,TABGET(stdtypes,STDTYPE_I));
	}
	else if ((parser->token[0]=='0')&&(parser->token[1]=='x')
		&&(ishexadecimal(parser->token+2)))	// gestion des entiers
	{
		int i=mtl_htoi(parser->token+2);
		bcint_byte_or_int(i);
		return STACKPUSH(m,TABGET(stdtypes,STDTYPE_I));
	}
	else if (parser->token[0]=='"')	// gestion des chaines
	{
		if (k=parser->getstring(m,'"')) return k;
		int val=INTTOVAL(nblabels(globals));
		if (k=addlabel(globals,"",val,STACKGET(m,0))) return k;	// enregistrement d'une nouvelle globale
		STACKDROP(m);
		bc_byte_or_int(VALTOINT(val),OPgetglobalb,OPgetglobal);
		return STACKPUSH(m,TABGET(stdtypes,STDTYPE_S));
	}
	else if (!strcmp(parser->token,"#"))	// gestion des pointeurs sur fonction statique ou dynamique
		return parsepntfun();
	else
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : unexpected term '%s'\n",parser->token);
		return MTLERR_SN;
	}
}

// parsing de la création d'une structure
// p contient la référence du premier champ
int Compiler::parsefields(int* p)
{
	int k;
	int* type=VALTOPNT(TABGET(VALTOPNT(TABGET(p,REF_VAL)),FIELD_TYPE));
	int n=VALTOINT(TABGET(type,REF_VAL));
	bc_byte_or_int(n,OPmktabb,OPmktab);	// création de la structure
	if (k=copytype(VALTOPNT(TABGET(type,REF_TYPE)))) return k;	// création du type

	int loop=1;
	while(loop)
	{
		if (k=copytype(VALTOPNT(TABGET(p,REF_TYPE)))) return k;	// création du type du champ
		if (k=unif(VALTOPNT(STACKGET(m,1)),
			VALTOPNT(TABGET(argsfromfun(VALTOPNT(STACKGET(m,0))),TYPEHEADER_LENGTH)) )) return k;
		STACKSET(m,0,TABGET(VALTOPNT(STACKGET(m,0)),TYPEHEADER_LENGTH+1));
		
		if (k=parser->parsekeyword(":")) return k;
		if (k=parseexpression()) return k;
		if (k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(STACKGET(m,1)))) return k;
		STACKDROPN(m,2);
		bc_byte_or_int( VALTOINT(TABGET(VALTOPNT(TABGET(p,REF_VAL)),FIELD_NUM)), OPsetstructb,OPsetstruct);

		
		if (!parser->next(0))
		{
			PRINTF(m)(LOG_COMPILER,"Compiler : ']' expected (found EOF)\n");
			return MTLERR_SN;
		}
		if (!strcmp(parser->token,"]"))
		{
			return 0;
		}
		loop=0;
		if (islabel(parser->token))
		{
			if ((p=searchref(PNTTOVAL(newpackage),parser->token))	// recherche dans les autres globales
				&&(VALTOINT(TABGET(p,REF_CODE))==CODE_FIELD))
				loop=1;
		}
	}
	PRINTF(m)(LOG_COMPILER,"Compiler : ']' expected (found '%s')\n",parser->token);
	return MTLERR_SN;
}


// parsing d'une référence quelconque (déjà présente dans parser->token)
int Compiler::parseref()
{
	int k;

	int val;
	int ref;
	if (!searchlabel_byname(locals,parser->token,&val,&ref))	// recherche dans les variables locales
	{
		bc_byte_or_int(VALTOINT(val),OPgetlocalb,OPgetlocal);
		if (k=STACKPUSH(m,ref)) return k;
		return parsegetpoint();
	}
	val=-1;

	int *p;
	if (p=searchref(PNTTOVAL(newpackage),parser->token))	// recherche dans les autres globales
	{
		ref=PNTTOVAL(p);
		int code=VALTOINT(TABGET(p,REF_CODE));
		if (code==CODE_CONS0)
		{
			bcint_byte_or_int(VALTOINT(TABGET(p,REF_VAL)));
			bc->addchar(OPdeftabb);
			bc->addchar(1);
			return copytype(VALTOPNT(TABGET(VALTOPNT(TABGET(p,REF_TYPE)),TYPEHEADER_LENGTH+1)));
		}
		else if (code==CODE_CONS)
		{
			bcint_byte_or_int(VALTOINT(TABGET(p,REF_VAL)));
			if (k=parseexpression()) return k;
			bc->addchar(OPdeftabb);
			bc->addchar(2);
			if (k=createnodetuple(1)) return k;
			if (k=copytype(VALTOPNT(TABGET(p,REF_TYPE)))) return k;
			return unif_argfun();
		}
		val=0;
	}

	if (val!=-1)
	{
		int* p=VALTOPNT(ref);
		int code=VALTOINT(TABGET(p,REF_CODE));

		if (code>=0)	// appel d'une fonction
		{
			int i; for(i=0;i<code;i++) if (k=parseexpression())
			{
				PRINTF(m)(LOG_COMPILER,"Compiler : function requires %d arguments\n",code);
				return k;
			}
			if ((TABGET(p,REF_VAL)!=NIL)&&(TABGET(VALTOPNT(TABGET(p,REF_VAL)),FUN_NBLOCALS)==NIL))
			{	// appel natif
				bc->addchar((char)VALTOINT(TABGET(VALTOPNT(TABGET(p,REF_VAL)),FUN_BC)));
			}
			else
			{	// appel normal
				bcint_byte_or_int(VALTOINT(TABGET(p,REF_PACKAGE)));
				bc->addchar(OPexec);
			}
			if (k=createnodetuple(code)) return k;
			if (p!=newref)
			{
				if (k=copytype(VALTOPNT(TABGET(p,REF_TYPE)))) return k;
			}
			else if (k=STACKPUSH(m,TABGET(p,REF_TYPE))) return k;
			return unif_argfun();
		}
		else if (code==CODE_VAR || code==CODE_CONST)	// lecture d'une référence
		{
			bc_byte_or_int(VALTOINT(TABGET(p,REF_PACKAGE)),OPgetglobalb,OPgetglobal);
			if (k=STACKPUSH(m,TABGET(p,REF_TYPE))) return k;
			return parsegetpoint();
		}
	}

	PRINTF(m)(LOG_COMPILER,"Compiler : unknown label '%s'\n",parser->token);
	return MTLERR_SN;
}

// parsing (.Term/champ)* en lecture
int Compiler::parsegetpoint()
{
	int k;
	while(1)
	{
		if (!parser->next(0)) return 0;
		if (strcmp(parser->token,"."))
		{
			parser->giveback();
			return 0;
		}
		if (!parser->next(0))
		{
			PRINTF(m)(LOG_COMPILER,"Compiler : expression or field name expected (found EOF)\n");
			return MTLERR_SN;
		}

		int* p;
		if ((islabel(parser->token))
			&&(p=searchref(PNTTOVAL(newpackage),parser->token))	// recherche dans les autres globales
			&&(VALTOINT(TABGET(p,REF_CODE))==CODE_FIELD))
		{
			bc_byte_or_int(VALTOINT(TABGET(VALTOPNT(TABGET(p,REF_VAL)),FIELD_NUM)),OPfetchb,OPfetch);
			if (k=createnodetuple(1)) return k;
			if (k=copytype(VALTOPNT(TABGET(p,REF_TYPE)))) return k;
			if (k=unif_argfun()) return k;
		}
		else
		{
			parser->giveback();
			if (k=parseterm()) return k;
			bc->addchar(OPfetch);
			if (k=createnodetuple(2)) return k;
			if (k=copytype(VALTOPNT(TABGET(stdtypes,STDTYPE_fun__tab_u0_I__u0)))) return k;
			if (k=unif_argfun()) return k;
		}
	}
}


// parsing du if ... then ... else (le 'if' a déjà été lu)
int Compiler::parseif()
{
	int k;
	
	if (k=parseexpression()) return k;	// lire la condition

	if (k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(TABGET(stdtypes,STDTYPE_I)))) return k;
	STACKDROP(m);
	if (k=parser->parsekeyword("then")) return k;

	bc->addchar(OPelse);
	int bc_else=bc->getsize();
	bc->addshort(0);	// on prépare le champ pour le saut 'else'

	if (k=parseexpression()) return k;	// lire l'expression 'then'

	bc->addchar(OPgoto);
	int bc_goto=bc->getsize();
	bc->addshort(0);	// on prépare le champ pour le saut du 'then'

	bc->setshort(bc_else,bc->getsize());	// on règle le saut du else

	if ((parser->next(0))&&(!strcmp(parser->token,"else")))
	{
		if (k=parseexpression()) return k;	// lire l'expression 'else'
	}
	else	// pas de else, on remplace par nil
	{
		parser->giveback();
		bc->addchar(OPnil);
		if (k=createnodetype(TYPENAME_UNDEF)) return k;
	}
	bc->setshort(bc_goto,bc->getsize());	// on règle le saut du 'goto'

	if (k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(STACKGET(m,1)))) return k;
	STACKDROP(m);
	return 0;

}

// parsing du while ... do ... (le 'while' a déjà été lu)
int Compiler::parsewhile()
{
	int k;
	
	bc->addchar(OPnil);	// on empile le premier résultat

	int bc_while=bc->getsize();		// on retient la position pour le saut 'while'
	if (k=parseexpression()) return k;	// lire la condition

	if (k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(TABGET(stdtypes,STDTYPE_I)))) return k;
	STACKDROP(m);
	if (k=parser->parsekeyword("do")) return k;
	bc->addchar(OPelse);
	int bc_end=bc->getsize();
	bc->addshort(0);	// on prépare le champ pour le saut 'end'
	bc->addchar(OPdrop);	// on ignore le résultat précédent

	if (k=parseexpression()) return k;	// lire l'expression 'do'

	bc->addchar(OPgoto);
	bc->addshort(bc_while);	// on retourne à la condition
	
	bc->setshort(bc_end,bc->getsize());	// on règle le saut 'end'

	return 0;
}


// parsing du for ...=... ; cond ; nextvalue do ... (le 'for' a déjà été lu)
int Compiler::parsefor()
{
	int k;
	
	if (!parser->next(0))
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : label expected (found EOF)\n");
		return MTLERR_SN;
	}
	if (!islabel(parser->token))
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : label expected (found '%s')\n",parser->token);
		return MTLERR_SN;
	}
	if (k=STRPUSH(m,parser->token)) return k;
	// [name_it]
	if (k=parser->parsekeyword("=")) return k;
	if (k=parseexpression()) return k;	// lire la valeur d'initialisation
	// [type_init name_it]
	int i=nblabels(locals);
	if (k=addlabel(locals,STRSTART(VALTOPNT(STACKGET(m,1))),INTTOVAL(i),STACKGET(m,0))) return k;
	if (i+1>nblocals) nblocals=i+1;	// nombre maximum de variables locales

	// [type_init name_it]
	bc_byte_or_int(i,OPsetlocalb,OPsetlocal);

	if (k=parser->parsekeyword(";")) return k;

	bc->addchar(OPnil);	// on empile le premier résultat

	int bc_cond=bc->getsize();		// on retient la position pour le saut 'while'
	if (k=parseexpression()) return k;	// lire la condition
	if (k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(TABGET(stdtypes,STDTYPE_I)))) return k;
	STACKDROP(m);

	bc->addchar(OPelse);
	int bc_end=bc->getsize();
	bc->addshort(0);	// on prépare le champ pour le saut 'end'

	if (!parser->next(0))
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : 'do' expected (found EOF)\n");
		return MTLERR_SN;
	}
	if (!strcmp(parser->token,";"))
	{
		bc->addchar(OPgoto);
		int bc_expr=bc->getsize();
		bc->addshort(0);	// on prépare le champ pour le saut 'expr'

		int bc_next=bc->getsize();
		if (k=parseexpression()) return k;	// lire la valeur next
		if (k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(STACKGET(m,1)))) return k;
		STACKDROP(m);
		bc_byte_or_int(i,OPsetlocalb,OPsetlocal);	// mise à jour de l'itérateur
		bc->addchar(OPgoto);
		bc->addshort(bc_cond);

		if (k=parser->parsekeyword("do")) return k;

		bc->setshort(bc_expr,bc->getsize());
		bc->addchar(OPdrop);	// on ignore le résultat précédent
		if (k=parseexpression()) return k;	// lire la valeur itérée
		bc->addchar(OPgoto);
		bc->addshort(bc_next);	// on retourne à l'itérateur
	}
	else
	{
		parser->giveback();
		if (k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(TABGET(stdtypes,STDTYPE_I)))) return k;
		if (k=parser->parsekeyword("do")) return k;
		bc->addchar(OPdrop);	// on ignore le résultat précédent
		if (k=parseexpression()) return k;	// lire la valeur itérée
		bc_byte_or_int(i,OPgetlocalb,OPgetlocal);	// i+1
		bc->addchar(OPintb);
		bc->addchar(1);
		bc->addchar(OPadd);
		bc_byte_or_int(i,OPsetlocalb,OPsetlocal);	// mise à jour de l'itérateur
		bc->addchar(OPgoto);
		bc->addshort(bc_cond);	// on retourne à la condition
	}
	bc->setshort(bc_end,bc->getsize());

	removenlabels(locals,1);

	STACKSET(m,2,STACKGET(m,0));
	STACKDROPN(m,2);

	return 0;
}

// parsing du match ... with ... (le 'match' a déjà été lu)
int Compiler::parsematch()
{
	int k;

	if (k=parseexpression()) return k;	// lire l'objet
	if (k=createnodetype(TYPENAME_UNDEF)) return k;	// préparer le type du résultat
	// [result src]

	if (k=parser->parsekeyword("with")) return k;

	int end;
	if (k=parsematchcons(&end)) return k;
	STACKSET(m,1,STACKGET(m,0));
	STACKDROP(m);
	return 0;
}

int Compiler::parsematchcons(int* end)
{
	int k;
	if (k=parser->parsekeyword("(")) return k;
	if (!parser->next(0))
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : constructor expected (found EOF)\n");
		return MTLERR_SN;
	}
	int* p=NULL;
	if (!strcmp(parser->token,"_"))	// cas par défaut
	{
		bc->addchar(OPdrop);
		if (k=parser->parsekeyword("->")) return k;
		if (k=parseprogram()) return k;	// lire le résultat
		// [type_result result src]
		if (k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(STACKGET(m,1)))) return k;
		STACKDROP(m);
		// [result src]
		if (k=parser->parsekeyword(")")) return k;
		*end=bc->getsize();
		return 0;
	}
	if ((islabel(parser->token))	// constructeur
		&&(p=searchref(PNTTOVAL(newpackage),parser->token)))	// recherche dans les autres globales
	{
		int code=VALTOINT(TABGET(p,REF_CODE));
		if ((code==CODE_CONS)||(code==CODE_CONS0))
		{
			// [result src]
			bc->addchar(OPfirst);
			bcint_byte_or_int(VALTOINT(TABGET(p,REF_VAL)));
			bc->addchar(OPeq);
			bc->addchar(OPelse);
			int bc_else=bc->getsize();		// on retient la position pour le saut 'else'
			bc->addshort(0);

			int nloc=nblabels(locals);	// on sauvegarde le nombre de locales
			if (code==CODE_CONS)
			{
				bc->addchar(OPfetchb);
				bc->addchar(1);
				if (k=parselocals()) return k;
				if (k=createnodetuple(1)) return k;
			}
			else
			{
				bc->addchar(OPdrop);
				if (k=createnodetuple(0)) return k;
			}
			// [[locals] result src]
			int newnloc=nblabels(locals);	// on sauvegarde le nombre de locales
			if (newnloc>nblocals) nblocals=newnloc;	// nombre maximum de variables locales
			int nloctodelete=newnloc-nloc;	// combien de variables locales ont été crées ?

			if (k=copytype(VALTOPNT(TABGET(p,REF_TYPE)))) return k;
			if (k=unif_argfun()) return k;
			// [type_src result src]
			if (k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(STACKGET(m,2)))) return k;
			STACKDROP(m);
			// [result src]
			if (k=parser->parsekeyword("->")) return k;
			if (k=parseprogram()) return k;	// lire le résultat
			// [type_result result src]
			if (k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(STACKGET(m,1)))) return k;
			STACKDROP(m);
			// [result src]
			removenlabels(locals,nloctodelete);
			if (k=parser->parsekeyword(")")) return k;
			bc->addchar(OPgoto);
			int bc_goto=bc->getsize();		// on retient la position pour le saut 'else'
			bc->addshort(0);
			bc->setshort(bc_else,bc->getsize());
			if ((parser->next(0))&&(!strcmp(parser->token,"|")))
			{
				if (k=parsematchcons(end)) return k;
				bc->setshort(bc_goto,*end);
				return 0;
			}
			parser->giveback();
			bc->addchar(OPdrop);
			bc->addchar(OPnil);
			*end=bc->getsize();
			bc->setshort(bc_goto,*end);
			return 0;
		}
	}
	PRINTF(m)(LOG_COMPILER,"Compiler : constructor expected (found '%s')\n",parser->token);
	return MTLERR_SN;
}


// parsing du let ... -> ... in (le 'let' a déjà été lu)
int Compiler::parselet()
{
	int k;
	
	if (k=parseexpression()) return k;	// lire la source
	if (k=parser->parsekeyword("->")) return k;

	int nloc=nblabels(locals);	// on sauvegarde le nombre de locales

	if (k=parselocals()) return k;

	int newnloc=nblabels(locals);	// on sauvegarde le nombre de locales
	if (newnloc>nblocals) nblocals=newnloc;	// nombre maximum de variables locales
	int nloctodelete=newnloc-nloc;	// combien de variables locales ont été crées ?

	if (k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(STACKGET(m,1)))) return k;
	STACKDROPN(m,2);
	if (k=parser->parsekeyword("in")) return k;

	if (k=parseexpression()) return k;	// lire l'expression du let
	removenlabels(locals,nloctodelete);
	return 0;
}


// parsing de variables locales structurées
int Compiler::parselocals()
{
	int k;
	
	if (!parser->next(0))
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : term expected (found EOF)\n");
		return MTLERR_SN;
	}
	else if (!strcmp(parser->token,"["))
    {
		int n=0;
		while(1)
        {
			if (!parser->next(0))
			{
				PRINTF(m)(LOG_COMPILER,"Compiler : ']' expected (found EOF)\n");
				return MTLERR_SN;
			}
			if (!strcmp(parser->token,"]"))
            {
				bc->addchar(OPdrop);
				return createnodetuple(n);
			}
			else if (!strcmp(parser->token,"_"))
            {
				if (k=createnodetype(TYPENAME_UNDEF)) return k;
				n++;
			}
			else
			{
				parser->giveback();
				bc->addchar(OPdup);
				bc_byte_or_int(n,OPfetchb,OPfetch);
				if (k=parselocals()) return k;
				n++;
			}
		}
	}
	else if (!strcmp(parser->token,"("))
    {
		if (k=createnodetype(TYPENAME_LIST)) return k;
		int* plist=VALTOPNT(STACKGET(m,0));
		if (k=createnodetype(TYPENAME_UNDEF)) return k;
		int* pval=VALTOPNT(STACKGET(m,0));
		TABSET(m,plist,TYPEHEADER_LENGTH,STACKGET(m,0));

		while(1)	// la liste est dans la pile
        {
			if (!parser->next(0)) return parselocals();
			if (!strcmp(parser->token,"_"))
            {
				if ((parser->next(0))&&(!strcmp(parser->token,")")))
				{
					bc->addchar(OPdrop);
					STACKDROP(m);
					return 0;
				}
				parser->giveback();
			}
			else if (islabel(parser->token))
			{
				int i=nblabels(locals);
				if (k=createnodetype(TYPENAME_UNDEF)) return k;
				if (k=addlabel(locals,parser->token,INTTOVAL(i),STACKGET(m,0))) return k;
				if ((parser->next(0))&&(!strcmp(parser->token,")")))
				{
					bc_byte_or_int(i,OPsetlocalb,OPsetlocal);
					if (k=unif(VALTOPNT(STACKGET(m,0)),plist)) return k;
					STACKDROPN(m,2);
					return 0;
				}
				parser->giveback();
				bc->addchar(OPfirst);
				bc_byte_or_int(i,OPsetlocalb,OPsetlocal);
				if (k=unif(VALTOPNT(STACKGET(m,0)),pval)) return k;
				STACKDROP(m);
			}
			else
			{
				parser->giveback();
				bc->addchar(OPfirst);
				if (k=parselocals()) return k;
				if (k=unif(VALTOPNT(STACKGET(m,0)),pval)) return k;
				STACKDROP(m);
			}
			if (!parser->next(0))
			{
				PRINTF(m)(LOG_COMPILER,"Compiler : '::' expected (found EOF)\n");
				return MTLERR_SN;
			}
			if (!strcmp(parser->token,"::"))
            {
				bc->addchar(OPfetchb);
				bc->addchar(1);
			}
			else
			{
				PRINTF(m)(LOG_COMPILER,"Compiler : '::' expected (found '%s')\n",parser->token);
				return MTLERR_SN;
			}
		}
	}
	else if (!strcmp(parser->token,"_"))
	{
		if (k=createnodetype(TYPENAME_UNDEF)) return k;
		bc->addchar(OPdrop);
		return 0;
	}
	else if (islabel(parser->token))
	{
		if (k=createnodetype(TYPENAME_UNDEF)) return k;
		int i=nblabels(locals);
		bc_byte_or_int(i,OPsetlocalb,OPsetlocal);

		if (k=addlabel(locals,parser->token,INTTOVAL(i),STACKGET(m,0))) return k;
		return 0;
	}
	PRINTF(m)(LOG_COMPILER,"Compiler : unexpected term '%s'\n",parser->token);
	return MTLERR_SN;
}

// parsing du update ... with ... (le 'update' a déjà été lu)
int Compiler::parseupdate()
{
	int k;
	
	if (k=parseexpression()) return k;	// lire la source
	if (k=parser->parsekeyword("with")) return k;
	if (k=parser->parsekeyword("[")) return k;

	if (k=parseupdatevals()) return k;

	if (k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(STACKGET(m,1)))) return k;
	STACKDROP(m);
	return 0;
}

// parsing des valeurs d'un update (le premier '[' a déjà été lu)
int Compiler::parseupdatevals()
{
	int k;

	int n=0;
	while(1)
    {
		if (!parser->next(0))
		{
			PRINTF(m)(LOG_COMPILER,"Compiler : ']' expected (found EOF)\n");
			return MTLERR_SN;
		}
		if (!strcmp(parser->token,"["))
		{
			bc->addchar(OPdup);
			bc_byte_or_int(n,OPfetchb,OPfetch);
			parseupdatevals();
			bc->addchar(OPdrop);
			n++;
		}
		else if (!strcmp(parser->token,"]"))
		{
			return createnodetuple(n);
		}
		else if (!strcmp(parser->token,"_"))
        {
			if (k=createnodetype(TYPENAME_UNDEF)) return k;
			n++;
		}
		else
		{
			parser->giveback();
			if (k=parseexpression()) return k;
			bc_byte_or_int(n,OPsetstructb,OPsetstruct);
			n++;
		}
	}
}


// parsing d'un set ... = ... (le 'set' a déjà été lu)
int Compiler::parseset()
{
	int k;
	int val;
	int ref;
	int opstore=-1;

	if (!parser->next(0))
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : reference expected (found EOF)\n");
		return MTLERR_SN;
	}
	if (!islabel(parser->token))
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : reference expected (found '%s')\n",parser->token);
		return MTLERR_SN;
	}

	if (!searchlabel_byname(locals,parser->token,&val,&ref))	// recherche dans les variables locales
	{
		if (k=STACKPUSH(m,ref)) return k;
		if (k=parsesetpoint(1,VALTOINT(val),&opstore)) return k;
	}
	else
	{
		val=-1;
			int *p;
			if (p=searchref(PNTTOVAL(newpackage),parser->token))	// recherche dans les autres globales
			{
				ref=PNTTOVAL(p);
				val=0;
			}
		if (val!=-1)
		{
			int* p=VALTOPNT(ref);
			int code=VALTOINT(TABGET(p,REF_CODE));

			if (code==CODE_VAR)	// variable
			{
				if (k=STACKPUSH(m,TABGET(p,REF_TYPE))) return k;
				if (k=parsesetpoint(0,VALTOINT(TABGET(p,REF_PACKAGE)),&opstore)) return k;

				// la variable a été settée au moins une fois maintenant
				TABSET(m,p,REF_SET,INTTOVAL(2));
			}
			else if (code == CODE_CONST) // constante
				{
					PRINTF(m)(LOG_COMPILER,"Compiler : '%s' is a const, it cannot be set\n",parser->token);
					return MTLERR_SN;
				}
		}
	}
	if (opstore==-1)
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : reference expected (found '%s')\n",parser->token);
		return MTLERR_SN;
	}
	if (k=parser->parsekeyword("=")) return k;
	if (k=parseexpression()) return k;	// lire la source
	bc->addchar(opstore);

	if (k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(STACKGET(m,1)))) return k;
	STACKDROP(m);
	return 0;
}

// parsing (.Term/champ)* en écriture
int Compiler::parsesetpoint(int local,int ind,int* opstore)
{
	int k;

	if (!parser->next(0)) return 0;
	if (strcmp(parser->token,"."))
	{
		bcint_byte_or_int(ind);
		if (local) *opstore=OPsetlocal2;
		else *opstore=OPsetglobal;
		parser->giveback();
		return 0;
	}
	if (local) bc_byte_or_int(ind,OPgetlocalb,OPgetlocal);
	else bc_byte_or_int(ind,OPgetglobalb,OPgetglobal);
	*opstore=OPstore;
	while(1)
	{
		if (!parser->next(0))
		{
			PRINTF(m)(LOG_COMPILER,"Compiler : expression or field name expected (found EOF)\n");
			return MTLERR_SN;
		}

		ind=-1;
		int* p;
		if ((islabel(parser->token))
			&&(p=searchref(PNTTOVAL(newpackage),parser->token))	// recherche dans les autres globales
			&&(VALTOINT(TABGET(p,REF_CODE))==CODE_FIELD))
		{
			ind=VALTOINT(TABGET(VALTOPNT(TABGET(p,REF_VAL)),FIELD_NUM));
			if (k=createnodetuple(1)) return k;
			if (k=copytype(VALTOPNT(TABGET(p,REF_TYPE)))) return k;
			if (k=unif_argfun()) return k;
		}
		else
		{
			parser->giveback();

			if (k=parseterm()) return k;
			if (k=createnodetuple(2)) return k;
			if (k=copytype(VALTOPNT(TABGET(stdtypes,STDTYPE_fun__tab_u0_I__u0)))) return k;
			if (k=unif_argfun()) return k;
		}
		if (!parser->next(0)) return 0;
		if (strcmp(parser->token,"."))
		{
			parser->giveback();
			if (ind>=0)
			{
				bcint_byte_or_int(ind);
			}
			return 0;
		}
		if (ind>=0) bc_byte_or_int(ind,OPfetchb,OPfetch);
		else bc->addchar(OPfetch);
	}
}

// parsing d'un pointeur de fonction
int Compiler::parsepntfun()
{
	int k;

	int val;
	int ref;

	if (!parser->next(0))
	{
		PRINTF(m)(LOG_COMPILER,"Compiler : function name expected (found EOF)\n");
		return MTLERR_SN;
	}
	val=-1;
		int *p;
		if (p=searchref(PNTTOVAL(newpackage),parser->token))	// recherche dans les autres globales
		{
			ref=PNTTOVAL(p);
			val=0;
		}
	if (val!=-1)
	{
		int* p=VALTOPNT(ref);
		int code=VALTOINT(TABGET(p,REF_CODE));

		if (code>=0)
		{
			int v=0;
			if (TABGET(p,REF_PACKAGE)!=NIL) v=VALTOINT(TABGET(p,REF_PACKAGE));
			else
			{
				char *ppp=parser->token;
				v=-VALTOINT(TABGET(VALTOPNT(TABGET(p,REF_VAL)),FUN_BC));
			}
			// pointeur d'une fonction virtuelle
			bcint_byte_or_int(v);
			if (p!=newref)
			{
				if (k=copytype(VALTOPNT(TABGET(p,REF_TYPE)))) return k;
			}
			else if (k=STACKPUSH(m,TABGET(p,REF_TYPE))) return k;
			return 0;
		}
	}

	PRINTF(m)(LOG_COMPILER,"Compiler : function name expected (found '%s')\n",parser->token);
	return MTLERR_SN;
}


// parsing du call ... ... (le 'call' a déjà été lu)
int Compiler::parsecall()
{
	int k;
	
	if (k=parseexpression()) return k;	// lire la fonction

	if ((parser->next(0))&&(!strcmp(parser->token,"[")))
	{
		int nval=0;
		while(1)
		{
			if (!parser->next(0))
			{
				PRINTF(m)(LOG_COMPILER,"Compiler : ']' expected (found EOF)\n");
				return MTLERR_SN;
			}
			if (!strcmp(parser->token,"]"))
			{
				bc_byte_or_int(nval,OPcallrb,OPcallr);
				if (k=createnodetuple(nval)) return k;
				if (k=createnodetuple(2)) return k;
				if (k=copytype(VALTOPNT(TABGET(stdtypes,STDTYPE_fun__fun_u0_u1_u0__u1)))) return k;
				return unif_argfun();
			}
			parser->giveback();
			if (k=parseexpression()) return k;
			nval++;
		}		
	}
	else
	{
		parser->giveback();
		if (k=parseexpression()) return k;	// lire les arguments

		bc->addchar(OPcall);
		if (k=createnodetuple(2)) return k;
		if (k=copytype(VALTOPNT(TABGET(stdtypes,STDTYPE_fun__fun_u0_u1_u0__u1)))) return k;
		return unif_argfun();
	}
}

