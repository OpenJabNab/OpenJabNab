// VLISP Virtual Machine - 2006 - by Sylvain Huet
// Lowcost IS Powerfull

#ifndef _LOADER_
#define _LOADER_

#define SYS_NB 6
#define SYS_CBPLAY 0
#define SYS_CBUDP 1
#define SYS_CBTCP 2
#define SYS_CBLOOP 3
#define SYS_ENV 4
#define SYS_CBREC 5

#define bytecode ((char*)vmem_heap)
//extern char bytecode[65536];
//extern char *bytecode;


extern char *bc_tabfun;
extern int bc_nbfun;
extern int sys_start;
extern int global_start;

int loaderGetInt(char *src);
int loaderGetShort(char *src);
int loaderFunstart(int funnumber);
int loaderInit(char *src);

#endif

