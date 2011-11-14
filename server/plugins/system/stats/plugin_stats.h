#ifndef _PLUGINSTATS_H_
#define _PLUGINSTATS_H_

#include "plugininterface.h"
	
class PluginStats : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginStats();
	virtual ~PluginStats();

	virtual void InitApiCalls();
	PLUGIN_API_CALL(Api_GetColors);
	PLUGIN_API_CALL(Api_GetPlugins);
	PLUGIN_API_CALL(Api_GetBunniesIP);
	PLUGIN_API_CALL(Api_GetBunniesName);
	PLUGIN_API_CALL(Api_GetBunniesStatus);
        PLUGIN_API_CALL(Api_GetBunniesInformation);
	
};

#endif
