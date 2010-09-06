#ifndef _PLUGINTAICHI_H_
#define _PLUGINTAICHI_H_

#include "plugininterface.h"
	
class PluginTaichi : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginTaichi();
	virtual ~PluginTaichi();

	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);
	void OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &);
	void SendTaichiFrequency(Bunny *);
	
	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_SetFrequency);
	PLUGIN_BUNNY_API_CALL(Api_GetFrequency);
};

#endif
