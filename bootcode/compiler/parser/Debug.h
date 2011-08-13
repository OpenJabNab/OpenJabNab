#ifndef __DEBUG__
#define __DEBUG__
#define DEBUG_ENABLED

#ifdef DEBUG_ENABLED
#define DEBUG(x)  cout << "From " << __FUNCTION__ << " : "; x ; cout << endl;
#else
#define DEBUG(x)
#endif
#endif
