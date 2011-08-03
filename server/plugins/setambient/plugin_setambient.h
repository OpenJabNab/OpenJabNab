#ifndef _PLUGINSETAMBIENT_H_
#define _PLUGINSETAMBIENT_H_

#include "plugininterface.h"

class PluginSetAmbient : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginSetAmbient();
	virtual ~PluginSetAmbient() {};
	void InitApiCalls();

	PLUGIN_BUNNY_API_CALL(Api_SendAmbient);
};

#endif
