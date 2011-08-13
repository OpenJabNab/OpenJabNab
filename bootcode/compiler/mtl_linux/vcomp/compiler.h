//-------------------
// MV
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 07/01/2003
// Derniere mise a jour : 07/01/2003
//

#ifndef _COMPILER_
#define _COMPILER_

#include"interpreter.h"

class Memory;
class Parser;
class Prodbuffer;

class Compiler
{
private:
	Memory* m;
	int systempackage;
	int* stdtypes;

	Prodbuffer* bc;	// structure de production de bytecode

	Prodbuffer* outputbuf;	// structure de production des messages de type


	Parser* parser;
	int* newpackage;
	int* newref;
	int globals;
	int locals;
	int nblocals;

	int ifuns;

	// typage
	int createnodetypecore(const char* name);
	int createnodetypecore(int name);
	int createnodetype(int type);
	int createnodetuple(int size);
	int createnodetupleval(int size);

	int* actualtype(int* p);

	int parsegraph(Parser* p,int env,int mono,int rec,int labels,int newvars,int* rnode);
	int parse_rnode(int* p);

	int creategraph(Parser* p,int env,int mono);
	int creategraph(Parser* p,int env,int mono,int labels);
	int creategraph(const char* src,int env,int mono);

	int recechograph(Prodbuffer *output,int* p,int rec,int labels);

	int reccopytype(int* p);
	int recresetcopy(int* p);

	int recgoweak(int* p);

	int restoreactual(int* t,int* s,int vt,int vs,int k);
	int recunif(int* x,int* y);
	int unif(int* x,int* y);
	int unif_argfun();

	int unifbigger(int* x,int* y);

	int* argsfromfun(int *f);

	void echonode(int code,int* p);
	// packages
	int hash(char* name);
	int createpackage(const char* name,int loghach);
	void addreftopackage(int* ref,int* package);
	int* searchtype(int env,char* name);
	int* searchemptytype(int env,char* name);

	void dumppackage(int env);

	int searchbytype(int env,int type);

	// liste de labels
	int addlabel(int base,const char* name,int val,int ref);
	int nblabels(int base);
	void removenlabels(int base,int n);
	int searchlabel_byname(int base,char* name,int* val,int* ref);
	int searchlabel_byval(int base,int val,char** name);
	int* tuplefromlabels(int base);

	// compilation
	int parsefile(int ifdef);
	int parsevar();
	int parseconst();
	int parseproto();
	int parseifdef(int ifndef);
	int skipifdef();
	int fillproto(int env,int* fun);
	int findproto(int env,int* fun);
	int parsetype();
	int parsestruct();
	int parsesum();

	int parsefun();
	int parseprogram();
	int parseexpression();
	int parsearithm();
	int parsea1();
	int parsea2();
	int parsea3();
	int parsea4();
	int parsea5();
	int parsea6();
	int parseterm();
	int parseref();
	int parseif();
	int parselet();
	int parseset();
	int parsewhile();
	int parsefor();
	int parsepntfun();
	int parselink();
	int parsecall();
	int parselocals();
	int parsestring();
	int parsefields(int* p);
	int parsematch();
	int parsematchcons(int* end);

	int parsegetpoint();
	int parsesetpoint(int local,int ind,int* opstore);

	int parseupdate();
	int parseupdatevals();

	// parsing variables
	int parseval();
	int parseval3();
	int parseval4();
	int parseval5();
	int parseval6();
	int parseval7();

	// production bytecode
	void bc_byte_or_int(int val,int opbyte,int opint);
	void bcint_byte_or_int(int val);
	// autres
	int addstdlibcore();
	int addstdlibstr();
	int addstdlibbuf();
	int addstdlibfiles();

	int recglobal(int val,Prodbuffer *b);
	int recglobals(int vlab,Prodbuffer *b);
	int recbc(int vref,Prodbuffer *b,Prodbuffer *btab,int offset);

public:
	Prodbuffer* brelease;
	Compiler(Memory* mem);
	~Compiler();
	int start();
	void stop();
	int addnative(int nref, const char** nameref, int* valref
              , int* coderef, const char** typeref,void* arg);

	int gocompile(int type); // [filename/src packages] -> [packages]
	int getsystempackage();

	int* searchref(int env,char* name);
	int* searchref_nosetused(int env,char* name);


	int echograph(Prodbuffer *output,int* p);
	int copytype(int* p);
	int recunifbigger(int* x,int* y);

};

#define LABELLIST_LENGTH 4
#define LABELLIST_NAME 0
#define LABELLIST_VAL 1
#define LABELLIST_REF 2
#define LABELLIST_NEXT 3

#define STDTYPE_LENGTH 8
#define STDTYPE_I 0
#define STDTYPE_F 1
#define STDTYPE_S 2
#define STDTYPE_Env 3
#define STDTYPE_Xml 4
#define STDTYPE_fun__u0_list_u0__list_u0 5
#define STDTYPE_fun__tab_u0_I__u0 6
#define STDTYPE_fun__fun_u0_u1_u0__u1 7


#define COMPILE_FROMFILE 0
#define COMPILE_FROMSTRING 1


#endif
