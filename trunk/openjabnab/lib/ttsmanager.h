#ifndef _TTSMANAGER_H_
#define _TTSMANAGER_H_

#include "global.h"

class OJN_EXPORT TTSManager
{
public:
	static bool CreateNewSound(QString, QString, QString, bool overwrite = false);
};

#endif
