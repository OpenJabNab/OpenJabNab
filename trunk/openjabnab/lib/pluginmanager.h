#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include <QVector>

#include "plugininterface.h"
#include "global.h"

class OJN_EXPORT PluginManager
{
public:
	PluginManager();
	virtual ~PluginManager();

	void HttpRequestBefore(HTTPRequest const&);
	bool HttpRequestHandle(HTTPRequest &);
	void HttpRequestAfter(HTTPRequest const&);
	
	void XmppBunnyMessage(QByteArray const&);
	void XmppVioletMessage(QByteArray const&);
	void XmppVioletPacketMessage(Packet & p);
	
	bool OnClick(PluginInterface::ClickType);
	bool OnEarsMove(int, int);
	
private:
	QVector<PluginInterface *> listOfPlugins;

};

#endif
