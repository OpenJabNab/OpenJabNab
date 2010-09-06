#ifndef _TTSMANAGER_H_
#define _TTSMANAGER_H_

#include "global.h"
#include "plugininterface.h"

class OJN_EXPORT TTSManager
{
public:
	static void Init();
	static QByteArray CreateNewSound(QString, QString, bool overwrite = false);
	
protected:
	static QStringList voiceList;
	static QDir ttsFolder;
	static QString ttsHTTPUrl;
};

#endif
