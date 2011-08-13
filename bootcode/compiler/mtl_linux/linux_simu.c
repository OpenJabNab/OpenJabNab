#define PROTORABBIT
//#define PROTODAL

// commenter la ligne suivante pour la version windows
//#define VMICROCHIP

typedef unsigned char uchar;
typedef unsigned long ulong;

#define vub unsigned char
#define vsb signed char

#define vuw unsigned short int
#define vsw signed short int

#define vud unsigned long
#define vsd signed long


void checkAllEvents(void);


#ifdef PROTORABBIT
#define VLISP_HARDWARE "NAB2"
#define NBLED 15
#define VL_MOTORS
#define VL_MUSIC
#define VL_3STATE
#define NBMOTOR 2
#endif

#ifdef PROTODAL
#define VLISP_HARDWARE "4"
#define NBLED 27
#define VL_PRESS
#endif

#ifdef VMICROCHIP
int simuInit(){ return 0;}
int simuDoLoop(){ return 0;}
void simuSetLed(vub i,vub val){}
void simuSetMotor(vub i,vub val){}

#else
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include "linux_simuaudio.h"
#include "linux_simunet.h"
#include "log.h"

// définition de l'échelle d'affichage de la simulation
#define SCALE 2

#define NBHOLES 20
#define MASKEDHOLES 3
// MAXMOTORVAL doit être inférieur à 256
#define MAXMOTORVAL 100

int motorwheel[256];

// gestion des couleurs
// ---------------------
uchar coloradd[256*256];
uchar colormul[256*256];

#define COLORADD(x,y) (coloradd[(x)+((y)<<8)])
#define COLORMUL(x,y) (colormul[(x)+((y)<<8)])

#define RGBTOINT(r,g,b) (( (((int)r)&255)<<16)+((((int)g)&255)<<8)+(((int)b)&255))
#define INTTOR(c) ((c>>16)&255)
#define INTTOG(c) ((c>>8)&255)
#define INTTOB(c) (c&255)

// initialisation des tables de couleur
void colortabInit()
{
	int i,j,k;

	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
		{
			k=i+j;
			coloradd[i+(j<<8)]=(k<=255)?k:255;

			k=i*j/255;
			colormul[i+(j<<8)]=k;
		}
}

// définition physique des leds

// en fait il y a NBLED/3 leds, mais on en met 3 fois plus pour
// pouvoir séparer chacune des composantes rgb de la couleur de chaque
// led
int diodeval[NBLED];
#ifdef VL_MOTORS
int motorval[NBMOTOR];
int motorcount[NBMOTOR];
int motordir[NBMOTOR];
#endif

int xclicsimu=-1;
int yclicsimu=-1;
int rclicsimu=0;
int movesimu=0;

int lastmovex=0;
int lastmovey=0;

#ifdef PROTORABBIT
int diodex[NBLED]={80,80,80,40,40,40,80,80,80,120,120,120,80,80,80};
int diodey[NBLED]={40,40,40,90,90,90,90,90,90,90,90,90,130,130,130};
int diodergb[NBLED]=
{0x0000ff,0x00ff00,0xff0000,
0x0000ff,0x00ff00,0xff0000,
0x0000ff,0x00ff00,0xff0000,
0x0000ff,0x00ff00,0xff0000,
0x0000ff,0x00ff00,0xff0000
};
#endif

#ifdef PROTODAL
int diodex[NBLED]={40,40,40,80,80,80,120,120,120,40,40,40,80,80,80,120,120,120,40,40,40,80,80,80,120,120,120};
int diodey[NBLED]={40,40,40,40,40,40,40,40,40,80,80,80,80,80,80,80,80,80,120,120,120,120,120,120,120,120,120};
int diodergb[NBLED]=
{0x0000ff,0x00ff00,0xff0000,
0x0000ff,0x00ff00,0xff0000,
0x0000ff,0x00ff00,0xff0000,
0x0000ff,0x00ff00,0xff0000,
0x0000ff,0x00ff00,0xff0000,
0x0000ff,0x00ff00,0xff0000,
0x0000ff,0x00ff00,0xff0000,
0x0000ff,0x00ff00,0xff0000,
0x0000ff,0x00ff00,0xff0000
};
#endif


// TODO
int getButton() { return 0; }

// fonction d'affichage des diodes
int simuDisplay(int* intensity)
{
	// TODO afficher l'état du lapin
	return 0;
}


// initialisation du simulateur
vsd simuInit()
{
	int i;
	colortabInit();

	for(i=0;i<NBLED;i++) diodeval[i]=255;
	srand(clock());
#ifdef VL_MOTORS
	for(i=0;i<NBMOTOR;i++)
	{
		motorval[i]=60;//(rand()&255)*MAXMOTORVAL/256;
		motorcount[i]=motordir[i]=0; 
	}
	for(i=0;i<256;i++) motorwheel[i]=0;
	for(i=0;i<MAXMOTORVAL;i++)
	{
		if ((i*2*NBHOLES/MAXMOTORVAL)&1) motorwheel[i]=1;
		if (i*NBHOLES/MAXMOTORVAL>=NBHOLES-MASKEDHOLES) motorwheel[i]=1;
	}
#endif
//	setButton(1);
	simuaudioinit();
	simunetinit();
	return 0;
}



// fonction à appeler régulièrement, pour traiter les messages de la fenêtre du simulateur
vsd simuDoLoop()
{
#ifdef VL_MOTORS
	int i,last;
#endif

	checkAllEvents();

#ifdef VL_MOTORS
	for(i=0;i<NBMOTOR;i++)
	{
		last=motorwheel[motorval[i]];
		if (1) motorval[i]+=motordir[i];
		if (motorval[i]<0) motorval[i]+=MAXMOTORVAL;
		if (motorval[i]>=MAXMOTORVAL) motorval[i]-=MAXMOTORVAL;
		if (last<motorwheel[motorval[i]]) motorcount[i]++;
	}
#endif
	simuDisplay(diodeval);
	return 0;
}


/**
	 Vérifie tous les évènements et fait les actions appropriées
 */
void checkAllEvents(void)
{
	checkNetworkEvents();
}


// réglagle d'une led
void simuSetLed(int i,int val)
{
	i*=3;
	if ((i<0)||(i>=NBLED)) return;
	diodeval[i]=(val>>16)&255;
	diodeval[i+1]=(val>>8)&255;
	diodeval[i+2]=(val)&255;

	my_printf(LOG_SIMULEDS, "Setting led %d: [%d %d %d]\n", i/3, diodeval[i], diodeval[i+1], diodeval[i+2]);
	my_printf(LOG_SIMULEDS, "All leds state:", i/3, diodeval[i], diodeval[i+1], diodeval[i+2]);
	for (i=0; i<NBLED/3; ++i)
		{
			my_printf(LOG_SIMULEDS, " [%3d %3d %3d]", diodeval[3*i], diodeval[3*i+1], diodeval[3*i+2]);
		}
	my_printf(LOG_SIMULEDS, "\n");
}

#ifdef VL_MOTORS
// réglagle d'un moteur
void set_motor_dir(int num_motor, int sens)
{
	int tmp_num, tmp_sens;
	tmp_num = num_motor?1:0;
	tmp_sens = (sens==0)?0:((sens==1)?1:-1);
	motordir[tmp_num]=tmp_sens;

	my_printf(LOG_SIMUMOTORS, "Setting motor %d, direction %d (pos: %d)\n", tmp_num, tmp_sens);
}

int get_motor_val(int i)
{
	int tmp_num = i?1:0;
	my_printf(LOG_SIMUMOTORS, "Getting value for motor %d: %d\n", tmp_num, motorcount[tmp_num]);
	return motorcount[tmp_num];
}
#else
int get_motor_val(int i)
{
	return 128;
}
#endif

int get_button3()
{
	// TODO
	return 0;
}

char buf_rfid[256];

char* get_rfid()
{
	// TODO
	return NULL;
}
#endif
