#ifndef _PLUGINLOCATE_H_
#define _PLUGINLOCATE_H_

#include <QObject>
#include "plugininterface.h"
#include "httprequest.h"
	
class PluginLocate : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	virtual bool HttpRequestHandle(HTTPRequest &);
};

#endif
