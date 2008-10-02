#ifndef _PLUGINBOOT_H_
#define _PLUGINBOOT_H_

#include "plugininterface.h"
#include "httprequest.h"
	
class PluginBoot : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginBoot();
	bool HttpRequestHandle(HTTPRequest &);
};

#endif
