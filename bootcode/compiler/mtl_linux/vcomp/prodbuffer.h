//-------------------
// MV
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 07/01/2003
// Derniere mise a jour : 07/01/2003
//

#ifndef _PRODBUFFER_
#define _PRODBUFFER_

class Prodbuffer
{
//public:
private:
	int size; // taille du buffer de bytecode
	char* buffer; // buffer de bytecode
	int index; // index du buffer de bytecode

	void bigger(); // augmentation de la taille du buffer

public:
	// buffer bytecode
	Prodbuffer();
	~Prodbuffer();
	void reinit();
	void addchar(char c);
	void addint(int i);
	void addshort(int i);
	void setchar(int index,char c);
	void setint(int index,int i);
	void setshort(int index,int i);
	void addstr(char *src,int len);
	void printf(const char *format, ...);
	void cut(int len);
	int getsize();
	char* getstart();
};

#define PRODBUFFER_FIRSTSIZE 128

#endif
