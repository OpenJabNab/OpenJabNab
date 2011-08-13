//-------------------
// MV
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 07/01/2003
// Derniere mise a jour : 07/01/2003
//

#ifndef _INTERPRETER_
#define _INTERPRETER_


class Memory;
class Prodbuffer;


class Interpreter
{
private:
	Memory* m;
	int callstack;
	char *pc; // pointeur vers le prochain bytecode
	char *bc; // pointeur vers le début du bytecode
	int locals; // pointeur vers les variable locales dans la pile
	int* globals; // pointeur vers les variable globales

public:
	Prodbuffer* output;

	Interpreter(Memory* memory);
	~Interpreter();
	int start();
	void stop();

	int go();

	int TFCtest(char* p);

int BCexec();
int BCret();
int BCdrop();
int BCdup();
int BCnil();
int BCgetlocal();
int BCsetlocal();
int BCgetglobal();
int BCgetglobalb();
int BCint();
int BCgoto();
int BCelse();
int BCfetch();
int BCstore();
int BCdeftab();
int BCnop();
int BCadd();
int BCsub();
int BCmul();
int BCdiv();
int BCneg();
int BCand();
int BCor();
int BCeor();
int BCnot();
int BCshl();
int BCshr();
int BCeq();
int BCne();
int BClt();
int BCgt();
int BCle();
int BCge();
int BCnon();
int BCsearch();
int BCfloat();
int BCcall();
int BCaddf();
int BCsubf();
int BCmulf();
int BCdivf();
int BCnegf();
int BCltf();
int BCgtf();
int BClef();
int BCgef();
int BCsetlocal2();
int BCsetglobal2();
int BCexecnative();
int BCgetlocalb();
int BCfetchb();
int BCdeftabb();
int BCsetlocalb();
int BCfirst();
int BCmod();
int BCmktab();
int BCmktabb();
int BCsetstruct();
int BCsetstructb();
int BCcallr();
int BCcallrb();
int BCgetglobalval();
int BCgetglobalvalb();
int BCupdate();
int BCupdateb();
int BCintb();

};

typedef int (Interpreter::*BCfunction)();
typedef int (*EXTfunction)(void*);

#include"../vbc.h"

#define CALLSTACK_LENGTH 6 // taille de la callstack sans les variables locales
#define CALLSTACK_BC 0	// pointeur bytecode
#define CALLSTACK_PC 1	// index bytecode
#define CALLSTACK_REF 2	// table références globales
#define CALLSTACK_PREV 3	// callstack précédente
#define CALLSTACK_NBARGS 4	// nombre d'arguments et variables globales
#define CALLSTACK_FUN 5	// pointeur vers la référence de la fonction en cours

#endif
