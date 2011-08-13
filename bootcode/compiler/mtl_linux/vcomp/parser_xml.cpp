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
#include "filesystem.h"
#include "file.h"
#include "parser.h"
#include "terminal.h"
#include "memory.h"
#include "util.h"
#include "interpreter.h"
#include "prodbuffer.h"

// lit un keyword imposé
int Parser::parsekeyword(const char* keyword)
{
	if (!next(0))
	{
		term->printf(LOG_COMPILER,"Parser : '%s' expected (found EOF)\n");
		return MTLERR_SN;
	}
	if (strcmp(token,keyword))
	{
		term->printf(LOG_COMPILER,"Parser : '%s' expected (found '%s')\n",keyword,token);
		return MTLERR_SN;
	}
	return 0;
}

// lit un keyword imposé
int Parser::parsekeyword(const char* keyword,int filter)
{
	if (!next(filter))
	{
		term->printf(LOG_COMPILER,"Parser : '%s' expected (found EOF)\n");
		return MTLERR_SN;
	}
	if (strcmp(token,keyword))
	{
		term->printf(LOG_COMPILER,"Parser : '%s' expected (found '%s')\n",keyword,token);
		return MTLERR_SN;
	}
	return 0;
}


// parsing d'une chaîne de caractères
int Parser::getstring(Memory* m,char separator)
{
	int c,n,i;
	
	char* name=token;
	name++;

	Prodbuffer* output=m->util->interpreter->output;
	output->reinit();

	n=0;
	while(1)
    {
		c=*(name++);
		if (c=='\\')
        {
			c=(*(name++))&255;
			if (c<32)
			{
				while(((*name)&255)<32) name++;
			}
			else
			{
				if (c=='n') c=10;
				else if (c=='z') c=0;
				else if (c=='$')
				{
					i=0;
					c=*name;
					if (ishex(c))
					{
						name++;
						i=htoc(c);
						c=*name;
						if (ishex(c))
						{
							name++;
							i=(i<<4)+htoc(c);
						}
					}
					c=i;
				}
				else if ((c>='0')&&(c<='9'))
			    {
					i=c-'0';
					c=*name;
					if ((c>='0')&&(c<='9'))
					{
						name++;
						i=(i*10)+c-'0';
						c=*name;
						if ((c>='0')&&(c<='9'))
						{
							name++;
							i=(i*10)+c-'0';
						}
					}
					c=i;
				}
				output->addchar(c);
            }
        }
		else if (c==separator)
        {
			return STRPUSHBINARY(m,output->getstart(),output->getsize());
        }
		else output->addchar(c);
    }
}
