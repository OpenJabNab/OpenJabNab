#ifndef _TTSMANAGER_H_
#define _TTSMANAGER_H_

#include <QEventLoop>
#include "global.h"

class OJN_EXPORT TTSManager
{
public:
	bool createNewSound(QString, QString, QString, bool overwrite = false);
};

#endif
