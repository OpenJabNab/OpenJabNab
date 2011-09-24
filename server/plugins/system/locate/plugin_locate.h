#ifndef _PLUGINLOCATE_H_
#define _PLUGINLOCATE_H_

#include "plugininterface.h"
#include "httprequest.h"
	
class PluginLocate : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginLocate();
	virtual ~PluginLocate() {};
	virtual bool HttpRequestHandle(HTTPRequest &);

	virtual void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_SetCustomLocateSetting);
	PLUGIN_BUNNY_API_CALL(Api_GetCustomLocateSetting);
};

#endif
