#ifndef _PLUGINBOOT_H_
#define _PLUGINBOOT_H_

#include <QObject>
#include "plugininterface.h"
#include "httprequest.h"
	
class PluginBOOT : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	bool HttpRequestHandle(HTTPRequest &);
};

#endif
