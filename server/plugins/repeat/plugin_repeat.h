#ifndef _PLUGINREPEAT_H_
#define _PLUGINREPEAT_H_

#include "plugininterface.h"
#include "httprequest.h"
	
class PluginRepeat : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginRepeat();
	virtual ~PluginRepeat();

	bool XmppVioletPacketMessage(Bunny *, Packet const&);
	bool OnClick(Bunny *, PluginInterface::ClickType);
};

#endif
