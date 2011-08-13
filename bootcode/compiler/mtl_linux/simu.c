/*
	PROJECT VLOWCOST 1.0
	copyright 2004
	Sylvain HUET
	amberMind
*/

#define PROTORABBIT
//#define PROTODAL

// commenter la ligne suivante pour la version windows
//#define VMICROCHIP


#define uchar unsigned char
#define ulong unsigned long
#ifndef uint
#define uint unsigned short int
#endif
#define vub unsigned char
#define vsb signed char

#define vuw unsigned short int
#define vsw signed short int

#define vud unsigned long
#define vsd signed long

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
#include<io.h>
#include<time.h>

#include"my_simuaudio.h"
#include"my_simunet.h"

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
HWND buttoncheck;
HWND button3;
HWND buttonrfid;
#define SUPP_HIGH 60
int width=160*SCALE;
int height=160*SCALE;
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
HWND buttoncheck;
#define SUPP_HIGH 20
int width=160*SCALE;
int height=160*SCALE;
#endif



#define FlagMainWindow (WS_VISIBLE|WS_CAPTION)
#define V_FONT ANSI_FIXED_FONT



HWND mainwin;
HBITMAP mainbmp;
char * mainbmppnt ;
int mainbmpnextline ;
DIBSECTION mainbmpinfo ;


void winrepaint()
{
	
    HDC Dcw , Dcb ;       
    HBITMAP OldBitmap ;
	PAINTSTRUCT ps;

	Dcw=BeginPaint(mainwin,&ps);
	Dcb=CreateCompatibleDC(Dcw);
	OldBitmap=SelectObject(Dcb,mainbmp);
	BitBlt(Dcw,0,0,width,height,Dcb,0,0,SRCCOPY);
	SelectObject(Dcb,OldBitmap);
	DeleteDC(Dcb);

	EndPaint(mainwin,&ps);
}

void winupdate()
{
	InvalidateRect(mainwin,NULL,FALSE);
    UpdateWindow(mainwin);
}

void mire()
{
	int i,j;
	char *pnt,*pnt0;
	int next;
	int increment=4;

	pnt0=mainbmppnt;
	next=mainbmpnextline;

	for(i=0;i<height;i++)
	{
		pnt=pnt0;
		for(j=0;j<width;j++)
		{
			pnt[0]=i&255;
			pnt[1]=j&255;
			pnt[2]=(i+j)&255;
			pnt+=increment;
		}
		pnt0+=next;
	}
}

void erasebmp()
{
	int i;
	char *pnt;
	int next;
	int increment=4;

	pnt=mainbmppnt;
	next=mainbmpnextline;

	for(i=0;i<height;i++)
	{
		memset(pnt,0,increment*width);
		pnt+=next;
	}
}

#define DIODELEN (20*SCALE)

void plot(int x,int y,int rgb)
{
	int r,g,b;
	uchar *pnt;
	int next;
	int increment=4;
	x*=SCALE;
	y*=SCALE;
	r=INTTOR(rgb);
	g=INTTOG(rgb);
	b=INTTOB(rgb);
	if ((x<0)||(y<0)||(x>=width)||(y>=height)) return;
	pnt=mainbmppnt;
	next=mainbmpnextline;
	pnt+=y*mainbmpnextline+x*increment;
	pnt[0]=r;
	pnt[1]=g;
	pnt[2]=b;
}

void diodedraw(int num,int val)
{
	int i,j,x,y,r,g,b,rgb;
	uchar *pnt,*pnt0;
	int next;
	int increment=4;

	if ((num<0)||(num>=NBLED)) return;
	x=diodex[num]*SCALE-DIODELEN/2;
	y=diodey[num]*SCALE-DIODELEN/2;
	rgb=diodergb[num];
	r=COLORMUL(INTTOR(rgb),(uchar)val);
	g=COLORMUL(INTTOG(rgb),(uchar)val);
	b=COLORMUL(INTTOB(rgb),(uchar)val);

	if ((x<0)||(y<0)||(x+DIODELEN>width)||(y+DIODELEN>height)) return;

	pnt0=mainbmppnt;
	next=mainbmpnextline;
	pnt0+=y*mainbmpnextline+x*increment;

	for(i=0;i<DIODELEN;i++)
	{
		pnt=pnt0;
		for(j=0;j<DIODELEN;j++)
		{
			pnt[0]=COLORADD(r,pnt[0]);
			pnt[1]=COLORADD(g,pnt[1]);
			pnt[2]=COLORADD(b,pnt[2]);
			pnt+=increment;
		}
		pnt0+=next;
	}
}

#define MOTORLEN (20*SCALE)

void motordraw(int x0,int val)
{
	int i,j,x,y,r,g,b,rgb;
	uchar *pnt,*pnt0;
	int next;
	int increment=4;

	x=x0*SCALE-MOTORLEN/2;
	y=(height*(val+128)>>9)-MOTORLEN/2;
	rgb=0xc0c0c0;
	r=INTTOR(rgb);
	g=INTTOG(rgb);
	b=INTTOB(rgb);

	if ((x<0)||(y<0)||(x+MOTORLEN>width)||(y+MOTORLEN>height)) return;

	pnt0=mainbmppnt;
	next=mainbmpnextline;
	pnt0+=y*mainbmpnextline+x*increment;

	for(i=0;i<MOTORLEN;i++)
	{
		pnt=pnt0;
		for(j=0;j<MOTORLEN;j++)
		{
			if (!((j+i+1)&3))
			{
				pnt[0]=COLORADD(r,pnt[0]);
				pnt[1]=COLORADD(g,pnt[1]);
				pnt[2]=COLORADD(b,pnt[2]);
			}
			pnt+=increment;
		}
		pnt0+=next;
	}
}

HWND crText (char *text,HWND Parent,int x, int y, int w,int h)
{
	HWND hwnd;
	int Flags=ES_READONLY|WS_CHILD | ES_RIGHT  ;

	hwnd=CreateWindowEx ( 0 ,
	"Edit" ,
	text , Flags ,
	x,y,w,h,
	Parent , NULL ,
	NULL/*thisinst*/ , NULL ) ;
	SendMessage(hwnd,WM_SETFONT,(WPARAM)GetStockFont(V_FONT),(LPARAM)TRUE);      

    ShowWindow(hwnd,SW_SHOW);
    UpdateWindow(hwnd);
	return hwnd;
}

HWND crEdit (char *text,HWND Parent,int x, int y, int w,int h)
{
	HWND hwnd;
	int Flags=WS_CHILD ;

	hwnd=CreateWindowEx ( WS_EX_CLIENTEDGE ,
	"Edit" ,
	text , Flags ,
	x,y,w,h,
	Parent , NULL ,
	NULL/*thisinst*/ , NULL ) ;
	SendMessage(hwnd,WM_SETFONT,(WPARAM)GetStockFont(V_FONT),(LPARAM)TRUE);      
    ShowWindow(hwnd,SW_SHOW);
    UpdateWindow(hwnd);
	return hwnd;
}

void setText(HWND h,int v)
{
	char buf[32];
	sprintf(buf,"%d",v);
	SetWindowText(h,buf);
}

char* getTextStr(HWND h,char *buf)
{
	int s;
	*(int*)buf=255;
	s=SendMessage(h,EM_GETLINE,0,(LPARAM)buf);
	buf[s]=0;
	return buf;
}

int getText(HWND h)
{
	char buf[256];
	int s;
	*(int*)buf=255;
	s=SendMessage(h,EM_GETLINE,0,(LPARAM)buf);
	buf[s]=0;
	return atoi(buf);
}


HWND crCheck (char *text,HWND Parent,int x, int y, int w,int h)
{
	HWND hwnd;
	int Flags=WS_CHILD|BS_AUTOCHECKBOX ;

	hwnd=CreateWindowEx ( 0 ,
	"button" ,
	text , Flags ,
	x,y,w,h,
	Parent , NULL ,
	NULL/*thisinst*/ , NULL ) ;
    ShowWindow(hwnd,SW_SHOW);
    UpdateWindow(hwnd);
	return hwnd;
}

void setCheck(HWND h,int v)
{
	SendMessage(h,BM_SETCHECK,v,0);
}

int getCheck(HWND h)
{
	return SendMessage(h,BM_GETCHECK,0,0);
}
#ifdef PROTORABBIT
void setButton(int v)
{
	setCheck(buttoncheck,v);
}
int getButton()
{
	return getCheck(buttoncheck);
}
#endif

#ifdef PROTODAL
void setButton(int v)
{
	setCheck(buttoncheck,v);
}
int getButton()
{
	return getCheck(buttoncheck);
}
#endif

LRESULT CALLBACK WindowProc( HWND hwnd, unsigned msg,
							UINT wParam, LONG lParam )
{
	int d,v,k,last;

    switch( msg ) {
		
    case WM_DESTROY:
		exit(0);//PostQuitMessage(0);
		break;
    case WM_PAINT:
        winrepaint();
        break;
    case WM_MOUSEMOVE :
		d=LOWORD(lParam)-lastmovex;
		movesimu+=(d>0)?d:-d;
		d=HIWORD(lParam)-lastmovey;
		movesimu+=(d>0)?d:-d;
		lastmovex=LOWORD(lParam);
		lastmovey=HIWORD(lParam);
		if (rclicsimu)
		{
			xclicsimu=LOWORD(lParam)*1000/width;
			yclicsimu=HIWORD(lParam)*1000/height;
			d=-1;
			if ((yclicsimu>=250)&&(yclicsimu<750))
			{
				if (xclicsimu<150) d=0;
				if (xclicsimu>850) d=1;
			}
			if (d>=0)
			{
				v=MAXMOTORVAL*(yclicsimu-250)/500;
				if (v<motorval[d])
					for(k=motorval[d];k>v;k--)
					{
						last=motorwheel[motorval[d]];
						motorval[d]--;
						if (last<motorwheel[motorval[d]]) motorcount[d]++;
					}
				else
					for(k=motorval[d];k<v;k++)
					{
						last=motorwheel[motorval[d]];
						motorval[d]++;
						if (last<motorwheel[motorval[d]]) motorcount[d]++;
					}
//				printf("move ear %d - %d\n",motorval[d],motorcount[d]);
			}
		}
        break;
    case WM_LBUTTONDOWN :
		rclicsimu=1;
		xclicsimu=LOWORD(lParam)*1000/width;
		yclicsimu=HIWORD(lParam)*1000/height;
		if ((xclicsimu>150)&&(xclicsimu<850))
		{
			setButton(1);
		}
		break;
    case WM_LBUTTONUP :
		rclicsimu=0;
		setButton(0);
		xclicsimu=-1;
		yclicsimu=-1;
		break;
	case MM_WOM_DONE:
		audioEventPlayData(msg,wParam,lParam);
		break;
	case MM_WIM_DATA:
		audioEventRecData(msg,wParam,lParam);
		break;
	case UDPEVENT:
		udpevent(msg,wParam,lParam);
		break;
	case TCPEVENT:
		tcpevent(msg,wParam,lParam);
		break;

    default:
        return( DefWindowProc( hwnd, msg, wParam, lParam ) );
    }
	
	return 0;
}


int initWindow()
{
	char buf[256];

	RECT r;
	r.bottom=height+SUPP_HIGH;
	r.right=width;
	r.left=0;
	r.top=0;

	AdjustWindowRect(&r,FlagMainWindow,FALSE);

	mainwin =  CreateWindow("GenericClass",TEXT("Vlisp - "VLISP_HARDWARE), FlagMainWindow,
        600,0,r.right-r.left,r.bottom-r.top,
        NULL,NULL,NULL,NULL);

    if (!mainwin)
	{
		wsprintf(buf,TEXT("%d"),GetLastError());
		MessageBox(NULL,TEXT("window error"),buf,MB_OK);
		return -1;
	}
    
    ShowWindow(mainwin,SW_SHOW);
    UpdateWindow(mainwin);
#ifdef PROTORABBIT
	crText("button=",mainwin,0,height,width/2,20);
	buttoncheck=crCheck("",mainwin,width/2,height,width/2,20);
	crText("b3=",mainwin,0,height+20,width/2,20);
	button3=crEdit("0",mainwin,width/2,height+20,width/2,20);
	crText("rfid=",mainwin,0,height+40,width/2,20);
	buttonrfid=crEdit("",mainwin,width/2,height+40,width/2,20);
#endif
#ifdef PROTODAL
	crText("button=",mainwin,0,height,width/2,20);
	buttoncheck=crCheck("",mainwin,width/2,height,width/2,20);
#endif
	return 0;
}

int initBitmap ( )
{
	BITMAPINFO Bi ;
	int tf ;
	HDC dc ;
	char buf[256];
	
	dc = NULL ;
	
	Bi.bmiHeader.biSize = sizeof ( BITMAPINFOHEADER ) ;
	Bi.bmiHeader.biWidth = width ;
	
	Bi.bmiHeader.biHeight = - height ;
	
	Bi.bmiHeader.biPlanes = 1 ;
	Bi.bmiHeader.biBitCount = 32 ;
	Bi.bmiHeader.biCompression = BI_RGB ;
	Bi.bmiHeader.biSizeImage = 0 ;
	Bi.bmiHeader.biXPelsPerMeter = 0 ;
	Bi.bmiHeader.biYPelsPerMeter = 0 ;
	Bi.bmiHeader.biClrUsed = 0 ;
	tf = DIB_RGB_COLORS ;
    Bi.bmiHeader.biClrImportant = Bi.bmiHeader.biClrUsed ;

//	Bi.bmiColors[0] = NULL ;

	mainbmp = CreateDIBSection ( dc , (BITMAPINFO * ) & Bi , tf , & mainbmppnt , NULL , 0 ) ;
	
    if (mainbmp == NULL ) 
	{
		wsprintf(buf,TEXT("%d"),GetLastError());
		MessageBox(NULL,TEXT("bitmap error"),buf,MB_OK);
		return -1;
	}
	
	GetObject ( mainbmp , sizeof( DIBSECTION ) , &mainbmpinfo ) ;
    mainbmpnextline=mainbmpinfo.dsBm.bmWidthBytes ;
	return 0 ;              
}    

static BOOL FirstInstance()
{
    WNDCLASS    wc;
    BOOL        rc;
	
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (LPVOID) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = NULL;
    wc.hIcon = 0;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = "GenericClass";
    rc = RegisterClass( &wc );
	
    return( rc );
}

// fonction d'affichage des diodes
int simuDisplay(int* intensity)
{
	int i;
	erasebmp();
	for(i=0;i<NBLED;i++)
	{
		diodedraw(i,intensity[i]);
#ifdef VL_MOTORS
		plot(2,(160*128)>>9,0xffffff);
		plot(2,(160*(128+256))>>9,0xffffff);
		plot(160-2,(160*128)>>9,0xffffff);
		plot(160-2,(160*(128+256))>>9,0xffffff);
		motordraw(15,motorval[0]*256/MAXMOTORVAL);
		motordraw(160-15,motorval[1]*256/MAXMOTORVAL);
#endif
		//		VPRINT("led %d = %d\n",i,intensity[i]);
	}

	winupdate();
	return 0;
}


// initialisation du simulateur
vsd simuInit()
{
	int i;
	colortabInit();

	for(i=0;i<NBLED;i++) diodeval[i]=255;
	srand(GetTickCount());
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
	FirstInstance();
	if (initBitmap ( )) return FALSE;
	mire();
	if (initWindow()) return FALSE;
	winupdate();
//	setButton(1);
	simuaudioinit();
	simunetinit();
	return 0;
}



// fonction à appeler régulièrement, pour traiter les messages de la fenêtre du simulateur
vsd simuDoLoop()
{
	MSG         msg;
#ifdef VL_MOTORS
	int i,last;
#endif
	while (PeekMessage(&msg, NULL,  0, 0, PM_REMOVE))   
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

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

// réglagle d'une led
void simuSetLed(int i,int val)
{
	i*=3;
	if ((i<0)||(i>=NBLED)) return;
	diodeval[i]=(val>>16)&255;
	diodeval[i+1]=(val>>8)&255;
	diodeval[i+2]=(val)&255;
}

#ifdef VL_MOTORS
// réglagle d'un moteur
void set_motor_dir(int num_motor, int sens)
{
	motordir[num_motor?1:0]=(sens==0)?0:((sens==1)?1:-1);
}

int get_motor_val(int i)
{
	return motorcount[i?1:0];
}
#else
int get_motor_val(int i)
{
	return 128;
}
#endif

int get_button3()
{
	return getText(button3);
}

char buf_rfid[256];

char* get_rfid()
{
	getTextStr(buttonrfid,buf_rfid);
	if (strlen(buf_rfid)) return buf_rfid;
	return NULL;
}
#endif
