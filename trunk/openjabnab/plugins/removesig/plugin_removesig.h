#ifndef _PLUGINREMOVESIG_H_
#define _PLUGINREMOVESIG_H_

#include "plugininterface.h"
#include "httprequest.h"
	
class PluginRemoveSig : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginRemoveSig();
	virtual ~PluginRemoveSig();

	bool XmppVioletPacketMessage(Bunny *, Packet const&);
	
};

#endif
