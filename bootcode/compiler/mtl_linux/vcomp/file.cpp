//-------------------
// Moteur 3d
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 03/09/2002
// Derniere mise a jour : 05/10/2002
//

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "param.h"
#include "filesystem.h"
#include "file.h"

///////////////
// File - gestion simple de fichiers


File::File(FileSystem *fs)
{
	filesystem=fs;
	file=NULL;
	sizeinternalbuffer=64;
	internalbuffer=new char[sizeinternalbuffer];
}

File::~File()
{
	if (file) fclose(file);
	delete internalbuffer;
}

int File::close()
{
	if (file) fclose(file);
	file=NULL;
	return 0;
}


int File::openread(const char* name)
{
	file=fopen(name,"rb");
	if (file) return 0;
	return -1;
}

char* File::getcontent(int *size)
{
	if (file==NULL) return NULL;

	fseek(file,0,SEEK_END);
	*size=ftell(file);
	fseek(file,0,SEEK_SET);

	char* buffer;
	buffer=new char[(*size)+1];
	fread((void*)buffer,1,*size,file);
	buffer[*size]=0;
	close();
	return buffer;
}
int File::openwrite(const char* name)
{
	file=fopen(name,"wb");
	if (file) return 0;
	return -1;
}
int File::write(const char *outbuf,int size)
{
	if (file==NULL) return 0;

	int k=fwrite((void*)outbuf,1,size,file);
	if (k<0) return -1;
	fflush(file);
	return 0;
}
