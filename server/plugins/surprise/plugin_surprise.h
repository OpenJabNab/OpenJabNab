#ifndef _PLUGINSURPRISE_H_
#define _PLUGINSURPRISE_H_

#include "plugininterface.h"
	
class PluginSurprise : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginSurprise();
	virtual ~PluginSurprise();

	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);
	void OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &);
};

#endif
