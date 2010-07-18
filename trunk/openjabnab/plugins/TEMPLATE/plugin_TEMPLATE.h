#ifndef _TEMPLATECLASSUPPER_H_
#define _TEMPLATECLASSUPPER_H_

#include "plugininterface.h"
	
class TEMPLATECLASS : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	TEMPLATECLASS();
	virtual ~TEMPLATECLASS();

	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);
	void OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &);
};

#endif
