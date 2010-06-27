#ifndef _PLUGINRECORD_H_
#define _PLUGINRECORD_H_

#include "plugininterface.h"
#include "httprequest.h"
	
class PluginRecord : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginRecord();
	virtual ~PluginRecord() {};
	virtual bool HttpRequestHandle(HTTPRequest &);
private:
	QDir recordFolder;
};

#endif
