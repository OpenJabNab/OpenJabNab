#ifndef _PLUGINRFID_H_
#define _PLUGINRFID_H_

#include "plugininterface.h"
#include "httprequest.h"
	
class PluginRFID : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginRFID();
	virtual ~PluginRFID() {};
	virtual bool HttpRequestHandle(HTTPRequest &);

	void InitApiCalls();
	PLUGIN_API_CALL(Api_GetLastTag);
	PLUGIN_API_CALL(Api_GetLastTagForBunny);
};

#endif
