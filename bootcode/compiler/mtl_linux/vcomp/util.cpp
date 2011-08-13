//-------------------
// MV
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 07/01/2003
// Derniere mise a jour : 07/01/2003
//


#include <stdio.h>

#include "param.h"
#include "filesystem.h"
#include "terminal.h"
#include "memory.h"

#include "parser.h"
#include "compiler.h"
#include "interpreter.h"
#include "util.h"

Util::Util(Memory* mem)
{
	m=mem;
	compiler=new Compiler(m);
	interpreter=new Interpreter(m);
}

Util::~Util()
{
	delete interpreter;
	delete compiler;
}

void Util::stop()
{
	interpreter->stop();
	compiler->stop();
}

int Util::start()
{
	int k;
	if (k=compiler->start()) return k;
	if (k=interpreter->start()) return k;
	return 0;
}

