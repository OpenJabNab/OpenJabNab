#ifndef _PLUGINVIOLETAPI_H_
#define _PLUGINVIOLETAPI_H_

#include "plugininterface.h"

class PluginVioletapi : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginVioletapi();
	virtual ~PluginVioletapi();

	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_enableVApi);
	PLUGIN_BUNNY_API_CALL(Api_disableVApi);
	PLUGIN_BUNNY_API_CALL(Api_getStatus);
	PLUGIN_BUNNY_API_CALL(Api_getToken);
};

#endif
