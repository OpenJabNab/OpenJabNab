#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include <QVector>
#include <QMap>
#include "global.h"
#include "plugininterface.h"

class PluginInterface;
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
	bool XmppVioletPacketMessage(Packet const& p);
	
	bool OnClick(Bunny *, PluginInterface::ClickType);
	bool OnEarsMove(Bunny *, int, int);
	bool OnRFID(Bunny *, QByteArray const&);
	
	QVector<PluginInterface *> const& GetListOfPlugins() { return listOfPlugins; }
	PluginInterface * GetPluginByName(QString name) { return listOfPluginsByName.value(name); }
	
private:
	QVector<PluginInterface *> listOfPlugins;
	QMap<QString, PluginInterface *> listOfPluginsByName;

};

#endif
