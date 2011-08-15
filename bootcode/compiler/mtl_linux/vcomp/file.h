//-------------------
// Moteur 3d
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 03/09/2002
// Derniere mise a jour : 05/10/2002
//



#ifndef _FILE_
#define _FILE_

// commenter pour utiliser les fichiers windows
#include <stdio.h>

class FileSystem;

class File
{
private :
	FileSystem *filesystem;
	FILE *file;
	char* internalbuffer;
	int	sizeinternalbuffer;
public:
	File(FileSystem *fs);
	~File();

	int openread(const char* name);
	char* getcontent(int *size);

	int openwrite(const char* name);
	int write(const char* outbuf,int size);

	int close();

};
#endif
