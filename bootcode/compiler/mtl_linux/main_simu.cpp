// VLISP Virtual Machine - 2006 - by Sylvain Huet
// Lowcost IS Powerfull


// #define DUMPBC

#define MAXSIZE_BYTECODE (128*1024)

#include<stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
extern "C" {
#include"vmem.h"
#include"vloader.h"
#include"vinterp.h"
#include"properties.h"
#include "log.h"

int simuInit();

// fonction à appeler régulièrement, pour traiter les messages de la fenêtre du simulateur
int simuDoLoop();

// réglagle d'une led
void simuSetLed(int i,int val);

// réglagle d'un moteur
void simuSetMotor(int i,int val);

}

int handle_options(int argc, char **argv);
void usage();


char srcbytecode[MAXSIZE_BYTECODE];


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


void loadbytecode(const char *src)
{
	FILE *f;
	int i,n;
	f=fopen(src,"rb");
	if (!f)
	{
		printf("file %s not found\n",src);
		return;
	}
	n=fread(srcbytecode,1,MAXSIZE_BYTECODE,f);
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


int vcompDoit(char *starter);

extern unsigned char dumpbc[];

int main(int argc,char **argv)
{
	PropLoad("config.txt");

	if (!handle_options(argc, argv))
		return -1;

	PropDump();

	if (!vcompDoit(PropGet("SOURCE")))
	{
		loadbytecode("foo.bin");

		vmemInit(0);

		if (!strcmp(PropGet("BOOT"),"firmware"))
		{
			loaderInit((char*)dumpbc);
		}
		else
		{
			loaderInit(srcbytecode);
		}
		vmemDumpShort();
		getchar();


		simuInit();

		VPUSH(INTTOVAL(0));
		interpGo();
		VPULL();
		while(1)
		{
			simuDoLoop();
			VPUSH(VCALLSTACKGET(sys_start,SYS_CBLOOP));
			if (VSTACKGET(0)!=NIL) interpGo();
			VPULL();

			usleep(50 * 1000);
		}
		getchar();
	}
	return 0;
}


/**
	 Retourne une valeur différente de 0 si la chaîne non vide passée en argument
	 représente un nombre, 0 sinon
 */
static int my_is_number(const char *s)
{
	while ('\0'!=*s && (*s>='0' && *s<='9')) s++;
	return (*s == '\0');
}

/**
	 Gère les options qui sont passées en ligne de commande
 */
int handle_options(int argc, char **argv)
{
	int i=1;
	int res=1;

	while (i<argc) {
		if (!strcmp(argv[i], "--help")) {
			usage();
			res=0;
			break;
		} else if (!strcmp(argv[i], "--mac")) {
			++i;
			if (i>=argc) { usage(); return 0; }
			PropSet("MAC", argv[i]);
		} else if (!strcmp(argv[i], "--boot")) {
			PropSet("BOOT", "firmware");
		} else if (!strcmp(argv[i], "--source")) {
			++i;
			if (i>=argc) { usage(); return 0; }
			PropSet("SOURCE", argv[i]);
		} else if (!strcmp(argv[i], "--logs")) {
			++i;
			if (i>=argc) { usage(); return 0; }
			my_printf_set_options(argv[i]);
		} else if (!strcmp(argv[i], "--logfile")) {
			++i;
			if (i>=argc) { usage(); return 0; }
			if (my_printf_set_logfile(argv[i]))
				{ res=0; break; }
		} else if (!strcmp(argv[i], "--maxlogsize")) {
			++i;
			if (i>=argc || !my_is_number(argv[i])) { usage(); return 0; }
			my_printf_set_max_log_size(atoi(argv[i]));
		} else if (!strcmp(argv[i], "--maxlogtime")) {
			++i;
			if (i>=argc || !my_is_number(argv[i])) { usage(); return 0; }
			my_printf_set_max_log_time(atoi(argv[i]));
		} else if (!strcmp(argv[i], "--dologtime")) {
			my_printf_set_do_log_time(1);
		} else {
			usage();
			res=0;
			break;
		}
		++i;
	}

	return res;
}

/**
	 Affiche les options disponibles pour le logiciel
 */
void usage()
{
	printf("Usage: mtl_simy [--help] [--mac MACADDR] [--boot] [--source SOURCE]\n" \
				 "          --help: affiche cette aide et quitte\n" \
				 "          --mac MACADDR: permet de preciser une adresse mac pour le lapin\n" \
				 "          --boot: comme si on mettait \"BOOT firmware\" dans config.txt\n" \
				 "          --source SOURCE: compile et charge le bytecode present dans le fichier SOURCE\n" \
				 "          --logs <log1>,...,<logn>: specifie les types de logs qu'on veut afficher, entre init,vm,simunet,simuleds et simuaudio, separes par des virgules\n" \
				 "          --logfile <filename>: specifie le fichier dans lequel ecrire les logs du lapin\n" \
				 "          --maxlogsize <n>: specifie qu'un fichier de log ne peut depasser <n> octets\n" \
				 "          --maxlogtime <n>: specifie qu'il doit y avoir une rotation du fichier de log au moins toutes les <n> secondes\n" \
				 "          --dologtime: specifie qu'on doit afficher sur chaque ligne de log le nombre de secondes ecoulees depuis le lancement du simulateur\n" \
				 "Toutes les options sont prioritaires sur les valeurs contenues dans config.txt\n");
}
