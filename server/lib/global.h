#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#if defined(WIN32)
	#if defined(OJN_MAKEDLL)
		#define OJN_EXPORT  __declspec(dllexport)
	#else
		#define OJN_EXPORT  __declspec(dllimport)
	#endif
#else
	#define OJN_EXPORT
#endif

#endif
