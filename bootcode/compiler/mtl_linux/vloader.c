// VLISP Virtual Machine - 2006 - by Sylvain Huet
// Lowcost IS Powerfull

#include <string.h>
#include"vmem.h"
#include"vloader.h"

char *bc_tabfun;
int bc_nbfun;
int sys_start;
int global_start;

int loaderGetByte(char *src)
{
	int i;
	i=(src[0]&255);
	return i;
}
int loaderGetShort(char *src)
{
	int i;
	i=((src[1]&255)<<8)+(src[0]&255);
	return i;
}
int loaderGetInt(char *src)
{
	int i;
	i=((src[3]&255)<<24)+((src[2]&255)<<16)+((src[1]&255)<<8)+(src[0]&255);
	return i;
}

/**
	 Charge les variables globales
 */
char *loaderInitRec(char *src)
{
	int l,i;
	
	l=loaderGetInt(src);
	src+=4;
	if (l==-1)
	{
		//		printf("nil\n",l>>1);
		VPUSH(NIL);
	}
	else if (l&1)
	{
		l>>=1;
		if (l&1)
		{
			l>>=1;
			//			printf("tuple %d\n",l);
			for(i=0;i<l;i++) src=loaderInitRec(src);
			VMKTAB(l);
		}
		else
		{
			l>>=1;
			//			printf("string taille %d\n",l);
			VPUSH(PNTTOVAL(VMALLOCSTR(src,l)));
			src+=l;
		}
	}
	else
	{
		//		printf("int %d\n",l>>1);
		VPUSH(l);
	}
	return src;
}

int loaderSizeBC(char *src)
{
	int n,b;
	n=loaderGetInt(src);
	src+=n;
	b=loaderGetInt(src);
	src+=4+b;
	n+=4+b;
	n+=2+(loaderGetShort(src)<<2);
	return n;
}

int loaderInit(char *src)
{
	int n,nw,i;
	char* src0;
	char* dst;
	
	n=loaderSizeBC(src);
	dst=(char*)vmem_heap;
	memcpy(dst,src,n);
	nw=(n+3)>>2;
	vmemInit(nw);
	
	src=(char*)vmem_heap;
	
	sys_start=vmem_stack-1;
	for(i=0;i<SYS_NB;i++) VPUSH(NIL);
	
	src0=src;
	n=loaderGetInt(src);
	src+=4;
	global_start=vmem_stack-1;
	while(((int)(src-src0))<n) src=loaderInitRec(src);
	
	n=loaderGetInt(src);
	src+=4;
	
	bc_tabfun=&bytecode[n+2]; // bytecode == vmem_heap == src ...
	bc_nbfun=loaderGetShort(src+n);
	n+=2+(bc_nbfun<<2);
	for(i=0;i<n;i++) bytecode[i]=src[i];
	vmemSetstart((n+3)>>2);
	
	return 0;
}


/**
	 Retourne l'adresse du début d'une fonction dans le bytecode
 */
int loaderFunstart(int funnumber)
{
	return loaderGetInt(bc_tabfun+(funnumber<<2));
}

