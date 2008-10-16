#ifndef _PLUGINTTS_H_
#define _PLUGINTTS_H_

#include "apimanager.h"
#include "plugininterface.h"

class PluginTTS : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginTTS();
	virtual ~PluginTTS() {};
	ApiManager::ApiAnswer * ProcessBunnyApiCall(Bunny *, Account const&, QByteArray const&, HTTPRequest const&);

private:
};

#endif
