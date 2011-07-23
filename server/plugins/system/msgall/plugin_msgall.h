#ifndef _PLUGINMSGALL_H_
#define _PLUGINMSGALL_H_

#include "plugininterface.h"

class PluginMsgall : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginMsgall();
	virtual ~PluginMsgall();
void InitApiCalls();
	PLUGIN_API_CALL(Api_Say);
};

#endif
