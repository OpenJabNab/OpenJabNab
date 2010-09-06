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

	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_Signature);
	PLUGIN_BUNNY_API_CALL(Api_Respiration);
};

#endif
