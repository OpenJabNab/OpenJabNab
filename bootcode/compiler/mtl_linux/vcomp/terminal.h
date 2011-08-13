//-------------------
// MV
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 07/01/2003
// Derniere mise a jour : 07/01/2003
//

#ifndef _TERMINAL_
#define _TERMINAL_

class File;
class FileSystem;
class Memory;
class Terminal
{
private:
	char* internalbuffer;
	int	sizeinternalbuffer;
public:
	Memory* m;
	Terminal();
	~Terminal();

	void echo(int mask,char *s);
	void echo(int mask,char *s,int size);
	void printf(int mask,const char *format, ...);
};

#endif
