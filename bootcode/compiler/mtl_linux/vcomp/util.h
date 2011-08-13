//-------------------
// Moteur 3d
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 03/09/2002
// Derniere mise a jour : 05/10/2002
//



#ifndef _UTIL_
#define _UTIL_

class Memory;
class Compiler;
class Interpreter;

class Util
{
private :
	Memory* m;
public:
	Util(Memory* mem);
	~Util();
	int start();
	void stop();

	Compiler* compiler;
	Interpreter* interpreter;
};
#endif
