// VLISP Virtual Machine - 2006 - by Sylvain Huet
// Lowcost IS Powerfull


#define DUMPBC


#include<stdio.h>
#include <string.h>
#include <unistd.h>
extern "C" {
#include"vmem.h"
#include"vloader.h"
#include"vinterp.h"
#include"properties.h"

char srcbytecode[65536];


void dump(uchar *src,int len)
{
  int i,j;
  char buffer[64];
  printf("\n");
  for(i=0;i<len;i+=16)
  {
    printf("%04x ",i);
    for(j=0;j<16;j++) if (i+j<len)
    {
      printf(buffer,"%02x ",src[i+j]);
    }
    else printf("   ");
    for(j=0;j<16;j++) if (i+j<len) printf("%c",(((src[i+j]>=32)&&(src[i+j]<128))?src[i+j]:'.'));
    printf("\n");
//    DelayMs(100);
  }
}


void loadbytecode(char *src)
{
	FILE *f;
	int i,n;
	f=fopen(src,"rb");
	if (!f)
	{
		printf("file %s not found\n",src);
		return;
	}
	n=fread(srcbytecode,1,65536,f);
	fclose(f);

#ifdef DUMPBC
	f=fopen("dumpbc.c","wb");
	if (f)
	{
		fprintf(f,"const unsigned char dumpbc[%d]={\n",n);
		for(i=0;i<n;i++)
		{
			fprintf(f,"0x%02x",srcbytecode[i]&255);
			if (i+1<n) fprintf(f,",");
			if (!((i+1)&0xf)) fprintf(f,"\n");
		}
		fprintf(f,"\n};\n");
		fclose(f);
	}
#endif
}

}


int StartMetal(const char *starter, const char* output, bool inSign);

extern unsigned char dumpbc[];

void usage(char* inProgram) {
	printf("Syntaxe: %s [-s] source output\n", inProgram);
}

int main(int argc,char **argv)
{
	bool signBinary = false;
	char* source = argv[1];
	char* output = argv[2];
	if ((argc != 3) && (argc != 4)) {
		usage(argv[0]);
		return 1;
	}
	if (argc == 4) {
		if (strcmp(argv[1], "-s") == 0) {
			signBinary = true;
			source = argv[2];
			output = argv[3];
		} else {
			usage(argv[0]);
			return 1;
		}
	}

	StartMetal(source, output, signBinary);
	return 0;
}

