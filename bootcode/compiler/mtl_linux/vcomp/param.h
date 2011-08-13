//-------------------
// MV
// version WIN32 et POCKETPC
// Sylvain Huet
// Premiere version : 07/01/2003
// Derniere mise a jour : 07/01/2003
//
#ifndef _PARAM_
#define _PARAM_


#define VLSB

//-------------------
// Parametres du benchmark

//++++++++
//Plateforme : commenter pour la version win32, decommenter pour la version pocketpc
//#define POCKETPC

//++++++++
//Precision des flottant : decommenter une des deux lignes

//#define myfloat double
#define mtl_float float

//++++++++
//Resolution de l'image : commenter pour utiliser des bmp 24 bits,
// decommenter pour avoir des bmp 32 bits (impossible sur pocketpc ipaq 3630

#define MODEBMP32


// Fin des parametres du benchmark
//-------------------

#ifdef POCKETPC

#define FlagMainWindow (WS_VISIBLE)
#define UNICODEDISPLAY
#define UNICODEFILENAME

#else

#define FlagMainWindow (WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX)

#endif

#ifdef MODEBMP32
#define BMPINC 4
#else
#define BMPINC 3
#endif

typedef unsigned char uchar;

#ifdef UNICODEDISPLAY
#define display_type unsigned short
#define display_strlen wcslen
#define display_copyfromchar(dst,src) wsprintf(dst,TEXT("%S"),src)
#define display_copy(dst,src) wcscpy(dst,src)
#else
#define display_type char
#define display_strlen strlen
#define display_copyfromchar(dst,src) strcpy(dst,src)
#define display_copy(dst,src) strcpy(dst,src)
#endif

#ifdef UNICODEFILENAME
#define filename_type unsigned short
#define filename_strlen wcslen
#define filename_copyfromchar(dst,src) wsprintf(dst,TEXT("%S"),src)
#define filename_copy(dst,src) wcscpy(dst,src)
#define filename_copyn(dst,src,n) {wcsncpy(dst,src,n);(dst)[n]=0;}
#define filename_strcmp(s1,s2) wcscmp(s1,s2)
#define filename_strncmp(s1,s2,n) wcsncmp(s1,s2,n)
#define filename_mkdir(d) _wmkdir(d)
#else
#define filename_type char
#define filename_strlen strlen
#define filename_copyfromchar(dst,src) strcpy(dst,src)
#define filename_copy(dst,src) strcpy(dst,src)
#define filename_copyn(dst,src,n) {strncpy(dst,src,n);(dst)[n]=0;}
#define filename_strcmp(s1,s2) strcmp(s1,s2)
#define filename_strncmp(s1,s2,n) strncmp(s1,s2,n)
#define filename_mkdir(d) _mkdir(d)

#endif

//#define API_PUB __declspec(dllexport)
#define API_PUB


#endif

