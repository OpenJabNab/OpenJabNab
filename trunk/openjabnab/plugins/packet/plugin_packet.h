#ifndef _PLUGINPACKET_H_
#define _PLUGINPACKET_H_

#include "plugininterface.h"
	
class PluginPacket : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginPacket();
	ApiManager::ApiAnswer * ProcessBunnyApiCall(Bunny *, Account const&, QString const&, HTTPRequest const&);
	virtual ~PluginPacket();
};

#endif
