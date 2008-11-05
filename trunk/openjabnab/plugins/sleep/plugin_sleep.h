#ifndef _PLUGINSLEEP_H_
#define _PLUGINSLEEP_H_

#include "plugininterface.h"
#include "httprequest.h"
	
class PluginSleep : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginSleep();
	virtual ~PluginSleep();

	ApiManager::ApiAnswer * ProcessBunnyApiCall(Bunny *, Account const&, QString const&, HTTPRequest const&);
	
};

#endif
