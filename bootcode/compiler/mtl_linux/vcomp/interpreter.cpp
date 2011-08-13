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
#include "prodbuffer.h"
#include "memory.h"
#include "util.h"
#include "interpreter.h"
#include "compiler.h"

#include"../vbc_str.h"

void displaybc(Memory* m,char* p)
{
	PRINTF(m)(LOG_DEVCORE,"dump %x\n",p);
	const char* spaces="         ";
	int ind=0;
	while(1)
	{
		int i=*p;
		if ((i<0)||(i>=MaxOpcode)) PRINTF(m)(LOG_DEVCORE,"%4d   ??\n",ind);
		else if ((i==OPint))
		{
			p++;
			int v=(p[0]&255)+((p[1]&255)<<8)+((p[2]&255)<<16)+((p[3]&255)<<24);
			PRINTF(m)(LOG_DEVCORE,"%4d   %s%s %d\n",ind,strbytecod[i],spaces+strlen(strbytecod[i]),v);
			ind+=4;
			p+=3;
		}
		else if ((i==OPgoto)||(i==OPelse))
		{
			p++;
			int v=(p[0]&255)+((p[1]&255)<<8);
			PRINTF(m)(LOG_DEVCORE,"%4d   %s%s %d\n",ind,strbytecod[i],spaces+strlen(strbytecod[i]),v);
			ind+=2;
			p+=1;
		}		else if ((i==OPgetlocalb)||(i==OPgetglobalb)||(i==OPfetchb)||(i==OPdeftabb)||(i==OPsetlocalb)||
			(i==OPmktabb)||(i==OPsetstructb)||(i==OPcallrb)||(i==OPintb))
		{
			p++;
			int v=p[0]&255;
			PRINTF(m)(LOG_DEVCORE,"%4d   %s%s %d\n",ind,strbytecod[i],spaces+strlen(strbytecod[i]),v);
			ind++;
		}
		else if (i==OPret)
		{
			PRINTF(m)(LOG_DEVCORE,"%4d   %s%s\n",ind,strbytecod[i],spaces+strlen(strbytecod[i]));
			return;
		}
		else PRINTF(m)(LOG_DEVCORE,"%4d   %s%s\n",ind,strbytecod[i],spaces+strlen(strbytecod[i]));
	ind++;
	p++;
	}
}


Interpreter::Interpreter(Memory* memory)
{
	m=memory;
	output=new Prodbuffer();
};

Interpreter::~Interpreter()
{
	delete output;
};

int Interpreter::start()
{
	return 0;
}

void Interpreter::stop()
{
}

